 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QIntValidator>

#ifdef Q_OS_WIN
#include <winsock2.h>
// name conflict..
#undef MessageBox
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/ignored-helper.h"
#include "gui/windows/message-box.h"
#include "gui/windows/password-window.h"
#include "protocols/protocols-manager.h"
#include "status/status.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "dcc/dcc-manager.h"
#include "helpers/gadu-formatter.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "gadu-account-details.h"
#include "gadu-contact.h"
#include "gadu-protocol-factory.h"

#include "gadu-protocol.h"

extern "C" int gadu_protocol_init(bool firstLoad)
{
	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	gg_debug_level = debug_mask | ~255;

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

#ifndef DEBUG_ENABLED
	gg_debug_level = 1;
#endif

	ProtocolsManager::instance()->registerProtocolFactory(GaduProtocolFactory::instance());

	if (!xml_config_file->hasNode("Accounts"))
		GaduImporter::instance()->importAccounts();

	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" void gadu_protocol_close()
{
	ProtocolsManager::instance()->unregisterProtocolFactory(GaduProtocolFactory::instance());
}

#define GG_STATUS_INVISIBLE2 0x0009
QString GaduProtocol::statusTypeFromGaduStatus(unsigned int index)
{
	switch (index)
	{
		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return "Online";

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return "Away";

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return "Invisible";

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:

		default:
			return "Offline";
	}
}

unsigned int GaduProtocol::gaduStatusFromStatus(const Status &status)
{
	bool hasDescription = !status.description().isEmpty();
	const QString &type = status.type();

	if ("Online" == type)
		return hasDescription ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;

	if ("Away" == type)
		return hasDescription ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;

	if ("Invisible" == type)
		return hasDescription ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;

	return hasDescription ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
}

GaduProtocol::GaduProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), Dcc(0),
		ActiveServer(), GaduLoginParams(), GaduSession(0), PingTimer(0)
{
	kdebugf();

	SocketNotifiers = new GaduProtocolSocketNotifiers(account, this);

	CurrentAvatarService = new GaduAvatarService(this);
	connect(this, SIGNAL(connected(Account)),
			this, SLOT(fetchAvatars(Account)));
	CurrentChatImageService = new GaduChatImageService(this);
	CurrentChatService = new GaduChatService(this);
	CurrentContactListService = new GaduContactListService(this);
	CurrentFileTransferService = new GaduFileTransferService(this);
	CurrentPersonalInfoService = new GaduPersonalInfoService(this);
	CurrentSearchService = new GaduSearchService(this);

	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	connect(BuddyManager::instance(), SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
	connect(BuddyManager::instance(), SIGNAL(contactAccountDataAdded(Buddy &, Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Buddy &, Account)));
	connect(BuddyManager::instance(), SIGNAL(contactAccountDataAboutToBeRemoved(Buddy &, Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Buddy &, Account)));

	kdebugf2();
}

void GaduProtocol::fetchAvatars(Account account)
{
	foreach (Buddy buddy, BuddyManager::instance()->buddies(account))
		if (buddy.hasContact(account))
			CurrentAvatarService->fetchAvatar(buddy.contact(account));
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	disconnect(BuddyManager::instance(), SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
	disconnect(BuddyManager::instance(), SIGNAL(contactAccountDataAdded(Buddy &, Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Buddy &, Account)));
	disconnect(BuddyManager::instance(), SIGNAL(contactAccountDataAboutToBeRemoved(Buddy &, Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Buddy &, Account)));

	networkDisconnected(false);
	delete SocketNotifiers;

	kdebugf2();
}

bool GaduProtocol::validateUserID(QString &uid)
{
	QIntValidator v(1, 999999999, this);
	int pos = 0;

	if (v.validate(uid, pos) == QValidator::Acceptable)
		return true;

	return false;
}

int GaduProtocol::maxDescriptionLength()
{
#ifdef GG_STATUS_DESCR_MAXSIZE_PRE_8_0
	if (GaduLoginParams.protocol_version <= 0x2a)
		return GG_STATUS_DESCR_MAXSIZE_PRE_8_0;
#endif

	return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::changeStatus()
{
	Status newStatus = nextStatus();

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		if (NetworkConnecting == state())
			networkDisconnected(false);
		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isDisconnected())
	{
		login();
		return;
	}

// TODO: 0.6.6
	int friends = (!newStatus.isDisconnected() && privateMode() ? GG_STATUS_FRIENDS_MASK : 0);
	int type = gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, unicode2cp(newStatus.description()));
	else
		gg_change_status(GaduSession, type | friends);

	printf("done\n");

	if (newStatus.isDisconnected())
		networkDisconnected(false);

	statusChanged(newStatus);
}

void GaduProtocol::changePrivateMode()
{
	changeStatus();
}

/*
void GaduProtocol::protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool /*last* /)
{
	kdebugf();*/
	/*
	   je�eli list� kontakt�w b�dziemy wysy�a� po kawa�ku, to serwer zgubi cz��� danych!
	   musimy wi�c wys�a� j� w ca�o�ci (poprzez sendUserList())
	   w takim w�a�nie przypadku (massively==true) nie robimy nic
	*/
/*
	Contact contact = elem.toContact(account());

	if (protocolName != "Gadu")
		return;
	if (status().isOffline())
		return;
	if (name != "OfflineTo" && name != "Blocking")
		return;

	if (massively)
	{
		sendUserListLater();
		return;
	}

	UinType contactUin = uin(contact);
	if (name == "OfflineTo")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_add_notify_ex(GaduSession, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_BLOCKED);
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_add_notify_ex(GaduSession, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_BLOCKED);
		}
	}
	else if (name == "Blocking")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_add_notify_ex(GaduSession, contactUin, GG_USER_BLOCKED);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_OFFLINE);
//			elem.setStatus(protocolName, GaduStatus());
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_add_notify_ex(GaduSession, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(GaduSession, contactUin, GG_USER_BLOCKED);
		}
	}

	kdebugf2();
}*/
/*
void GaduProtocol::userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool /*last* /)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;
#include <status/status-group.h>
#include <status/status-group.h>

	if (status().isOffline())
		return;
	if (name != "Anonymous")
		return;

	if (massively)
		sendUserListLater();
	else
	{
		if (!currentValue.toBool() && oldValue.toBool())
			gg_add_notify(GaduSession, uin(contact));
	}
	kdebugf2();
}*/
/*
void GaduProtocol::userAdded(UserListElement elem, bool massively, bool /*last* /)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", qPrintable(elem.altNick()), massively/*, last* /);

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;

	if (status().isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_add_notify(GaduSession, uin(contact));
	kdebugf2();
}*/
/*
void GaduProtocol::removingUser(UserListElement elem, bool massively, bool /*last* /)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", qPrintable(elem.altNick()), massively/*, last* /);

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;

	if (status().isOffline())
		return;
	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_remove_notify(GaduSession, uin(contact));
	kdebugf2();
}*/
/*
void GaduProtocol::protocolAdded(UserListElement elem, QString protocolName, bool massively, bool /*last * /)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (contact.isNull())
		return;

	if (protocolName != "Gadu")
		return;
	if (status().isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_add_notify(GaduSession, uin(contact));
	kdebugf2();
}*/
/*
void GaduProtocol::removingProtocol(UserListElement elem, QString protocolName, bool massively, bool /*last* /)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (contact.isNull())
		return;

	if (protocolName != "Gadu")
		return;
	if (status().isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_remove_notify(GaduSession, uin(contact));
	kdebugf2();
}*/

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();

	kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
	socketConnFailed(ConnectionTimeout);

	kdebugf2();
}

void GaduProtocol::everyMinuteActions()
{
	kdebugf();

	gg_ping(GaduSession);
	CurrentChatImageService->resetSendImageRequests();
}

void GaduProtocol::login(const QString &password, bool permanent)
{
	account().setPassword(password);
	account().setRememberPassword(permanent);

	login();
}

void GaduProtocol::login()
{
	kdebugf();

	if (GaduSession)
		return;

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());

	if (0 == gaduAccountDetails->uin())
	{
		MessageBox::msg(tr("UIN not set!"), false, "Warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: uin or password not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		PasswordWindow::getPassword(tr("Please provide password for %1 account").arg(account().name()),
				this, SLOT(login(const QString &, bool)));
		return;
	}

	networkStateChanged(NetworkConnecting);

	setupProxy();
	setupDcc();
	setupLoginParams();

	GaduSession = gg_login(&GaduLoginParams);

	cleanUpLoginParams();

	if (GaduSession)
		SocketNotifiers->watchFor(GaduSession);
	else
		networkDisconnected(false);

	kdebugf2();
}

void GaduProtocol::setupProxy()
{
	kdebugf();

	if (gg_proxy_host)
	{
		free(gg_proxy_host);
		gg_proxy_host = 0;
	}

	if (gg_proxy_username)
	{
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = 0;
	}

	gg_proxy_enabled = account().useProxy();

	if (gg_proxy_enabled)
	{
		gg_proxy_host = strdup((char *)unicode2latin(account().proxyHost().toString()).data());
		gg_proxy_port = account().proxyPort();

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

		if (account().proxyRequiresAuthentication() && !account().proxyUser().isEmpty())
		{
			gg_proxy_username = strdup((char *)unicode2latin(account().proxyUser()).data());
			gg_proxy_password = strdup((char *)unicode2latin(account().proxyPassword()).data());
		}
	}

	kdebugf2();
}

void GaduProtocol::setupDcc()
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	if (gaduAccountDetails->allowDcc())
	{
		if (!Dcc)
			Dcc = new DccManager(this);
	}
	else
	{
		if (Dcc)
		{
			delete Dcc;
			Dcc = 0;
		}
	}
}

void GaduProtocol::setupLoginParams()
{
	memset(&GaduLoginParams, 0, sizeof(GaduLoginParams));

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	GaduLoginParams.uin = account().id().toULong();
	GaduLoginParams.password = strdup(account().password().toAscii().data());

	GaduLoginParams.async = 1;
	GaduLoginParams.status = gaduStatusFromStatus(nextStatus()); // TODO: 0.6.6 support is friend only
	if (!nextStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup((const char *)unicode2cp(nextStatus().description()).data());

	ActiveServer = GaduServersManager::instance()->getServer();
	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	GaduLoginParams.protocol_version = 0x2a; // we are gg 7.7 now
	GaduLoginParams.client_version = (char *)"7, 7, 0, 3351";

	GaduLoginParams.has_audio = gaduAccountDetails->allowDcc();
	GaduLoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	if (Dcc)
		Dcc->setUpExternalAddress(GaduLoginParams);

	GaduLoginParams.tls = 0;
	GaduLoginParams.image_size = config_file.readUnsignedNumEntry("Chat", "MaxImageSize", 0);
}

void GaduProtocol::cleanUpLoginParams()
{
	memset(GaduLoginParams.password, 0, strlen(GaduLoginParams.password));
	free(GaduLoginParams.password);
	GaduLoginParams.password = 0;

	if (GaduLoginParams.status_descr)
	{
		free(GaduLoginParams.status_descr);
		GaduLoginParams.status_descr = 0;
	}
}

void GaduProtocol::networkConnected()
{
	networkStateChanged(NetworkConnected);
}

void GaduProtocol::networkDisconnected(bool tryAgain)
{
	if (!tryAgain)
		networkStateChanged(NetworkDisconnected);

	if (Dcc)
	{
		delete Dcc;
		Dcc = 0;
	}

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

	SocketNotifiers->watchFor(0); // stop watching

	if (GaduSession)
	{
		gg_free_session(GaduSession);
		GaduSession = 0;
	}

	setAllOffline();

	if (tryAgain && !nextStatus().isDisconnected()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	else if (!nextStatus().isDisconnected())
		setStatus(Status());
}

int GaduProtocol::notifyTypeFromContact(Buddy &buddy)
{
	return buddy.isOfflineTo(account())
		? GG_USER_OFFLINE
		: buddy.isBlocked(account())
			? GG_USER_BLOCKED
			: GG_USER_NORMAL;
}

void GaduProtocol::sendUserList()
{
	kdebugf();

	UinType *uins;
	char *types;

	BuddyList buddies = BuddyManager::instance()->buddies(account());

	if (buddies.isEmpty())
	{
		gg_notify_ex(GaduSession, 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = new UinType[buddies.count()];
	types = new char[buddies.count()];

	int i = 0;

	foreach (Buddy buddy, buddies)
	{
		uins[i] = uin(buddy);
		types[i] = notifyTypeFromContact(buddy);
		++i;
	}

	gg_notify_ex(GaduSession, uins, types, buddies.count());
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;

	kdebugf2();
}

void GaduProtocol::socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Buddy buddy = BuddyManager::instance()->byId(account(), QString::number(uin));

	if (buddy.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		emit userStatusChangeIgnored(buddy);
		gg_remove_notify(GaduSession, uin);
		return;
	}

	GaduContact *accountData = gaduContactAccountData(buddy);
	accountData->setIp(ip);
	accountData->setPort(port);
	accountData->setMaxImageSize(maxImageSize);
	accountData->setProtocolVersion(QString::number(version));
	accountData->setGaduProtocolVersion(version);

	Status oldStatus = accountData->status();
	Status newStatus;
	newStatus.setType(statusTypeFromGaduStatus(status));
	newStatus.setDescription(description);
	accountData->setStatus(newStatus);

	emit buddyStatusChanged(account(), buddy, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
	kdebugf();
	QString msg = QString::null;

	bool tryAgain = true;
	switch (error)
	{
		case ConnectionServerNotFound:
			msg = tr("Unable to connect, server has not been found");
			break;

		case ConnectionCannotConnect:
			msg = tr("Unable to connect");
			break;

		case ConnectionNeedEmail:
			msg = tr("Please change your email in \"Change password / email\" window. "
				"Leave new password field blank.");
			tryAgain = false;
			MessageBox::msg(msg, false, "Warning");
			break;

		case ConnectionInvalidData:
			msg = tr("Unable to connect, server has returned unknown data");
			break;

		case ConnectionCannotRead:
			msg = tr("Unable to connect, connection break during reading");
			break;

		case ConnectionCannotWrite:
			msg = tr("Unable to connect, connection break during writing");
			break;

		case ConnectionIncorrectPassword:
			msg = tr("Unable to connect, incorrect password");
			tryAgain = false;
			MessageBox::msg(tr("Connection will be stopped\nYour password is incorrect!"), false, "Critical");
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			tryAgain = false;
			MessageBox::msg(tr("Connection will be stopped\nToo many attempts with bad password"), false, "Critical");
			break;

		case ConnectionUnavailableError:
			msg = tr("Unable to connect, servers are down");
			break;

		case ConnectionUnknow:
			msg = tr("Connection broken");
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occured");
			break;

		default:
			kdebugm(KDEBUG_ERROR, "Unhandled error? (%d)\n", int(error));
			msg = tr("Connection broken");
			break;
	}

	if (!msg.isEmpty())
	{
		QHostAddress server = ActiveServer.first;
		QString host;
		if (!server.isNull())
			host = QString("%1:%2").arg(server.toString()).arg(ActiveServer.second);
		else
			host = "HUB";
		kdebugm(KDEBUG_INFO, "%s %s\n", qPrintable(host), qPrintable(msg));
		emit connectionError(account(), host, msg);
	}

	if (tryAgain)
		GaduServersManager::instance()->markServerAsBad(ActiveServer);
	networkDisconnected(tryAgain);

	kdebugf2();
}

void GaduProtocol::socketConnSuccess()
{
	kdebugf();

	sendUserList();

	GaduServersManager::instance()->markServerAsGood(ActiveServer);

	PingTimer = new QTimer(0);
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	statusChanged(nextStatus());
	networkConnected();

	// workaround about servers errors
	if ("Invisible" == status().type())
		setStatus(status());

	kdebugf2();
}

void GaduProtocol::socketDisconnected()
{
	kdebugf();

	networkDisconnected(false);

	kdebugf2();
}

unsigned int GaduProtocol::uin(Buddy buddy) const
{
	GaduContact *data = gaduContactAccountData(buddy);
	return data
		? data->uin()
		: 0;
}

GaduContact * GaduProtocol::gaduContactAccountData(Buddy buddy) const
{
	return dynamic_cast<GaduContact *>(buddy.contact(account()));
}

QPixmap GaduProtocol::statusPixmap(Status status)
{
	QString description = status.description().isEmpty()
			? ""
			: "WithDescription";
	QString pixmapName;

	QString groupName = status.group();

	if ("Away" == groupName)
		pixmapName = QString("Busy").append(description);
	else
		pixmapName = QString(groupName).append(description);

	return IconsManager::instance()->loadPixmap(pixmapName);
}

QPixmap GaduProtocol::statusPixmap(const QString &statusType)
{
	return IconsManager::instance()->loadPixmap(
			"Away" == statusType ? "Busy" : statusType);
}

void GaduProtocol::contactAdded(Buddy &buddy)
{
	GaduContact *gcad = gaduContactAccountData(buddy);
	if (!gcad)
		return;

	gg_add_notify_ex(GaduSession, gcad->uin(), notifyTypeFromContact(buddy));
}

void GaduProtocol::contactRemoved(Buddy &buddy)
{
	GaduContact *gcad = gaduContactAccountData(buddy);
	if (!gcad)
		return;

//	TODO: 0.6.6 which one is the *right* way?
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_NORMAL);
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_BLOCKED);
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_OFFLINE);
	gg_remove_notify(GaduSession, gcad->uin());
}

void GaduProtocol::contactAccountDataAdded(Buddy &buddy, Account contactAccount)
{
	if (contactAccount != account())
		return;

	contactAdded(buddy);
}

void GaduProtocol::contactAccountDataAboutToBeRemoved(Buddy &buddy, Account contactAccount)
{
	if (contactAccount != account())
		return;

	contactRemoved(buddy);
}

void GaduProtocol::contactAccountDataIdChanged(Buddy &buddy, Account contactAccount, const QString &oldId)
{
	if (contactAccount != account())
		return;

	bool ok;
	UinType oldUin = oldId.toInt(&ok);
	if (ok)
		gg_remove_notify(GaduSession, oldUin);

	UinType newUin = uin(buddy);
	if (newUin)
		gg_add_notify_ex(GaduSession, newUin, notifyTypeFromContact(buddy));
}
