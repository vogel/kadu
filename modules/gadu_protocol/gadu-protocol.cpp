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
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "buddies/ignored-helper.h"
#include "gui/windows/message-dialog.h"
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
#include "gadu-contact-details.h"
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

Buddy GaduProtocol::searchResultToBuddy(gg_pubdir50_t res, int number)
{
	Buddy result = Buddy::create();

	Contact contact = Contact::create();
	contact.setContactAccount(account());
	contact.setOwnerBuddy(result);
	contact.setId(gg_pubdir50_get(res, number, GG_PUBDIR50_UIN));
	contact.setDetails(new GaduContactDetails(contact));

	const char *pubdirStatus = gg_pubdir50_get(res, number, GG_PUBDIR50_STATUS);
	if (pubdirStatus)
	{	Status status;
		status.setType(statusTypeFromGaduStatus(atoi(pubdirStatus) & 127));
		contact.setCurrentStatus(status);
	}

	result.setFirstName(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_LASTNAME)));
	result.setNickName(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromAscii(gg_pubdir50_get(res, number, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_CITY)));
	result.setFamilyName(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(cp2unicode(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((BuddyGender)QString::fromAscii(gg_pubdir50_get(res, number, GG_PUBDIR50_GENDER)).toUShort());

	return result;
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

	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	kdebugf2();
}

void GaduProtocol::fetchAvatars(Account account)
{
	foreach (const Contact &contact, ContactManager::instance()->contacts(account))
		CurrentAvatarService->fetchAvatar(contact);
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

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
	account().setHasPassword(!password.isEmpty());

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
		MessageDialog::msg(tr("UIN not set!"), false, "Warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: gadu UIN not set\n");
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

int GaduProtocol::notifyTypeFromContact(const Contact &contact)
{
	Buddy buddy = contact.ownerBuddy();
	return buddy.isOfflineTo()
		? GG_USER_OFFLINE
		: buddy.isBlocked()
			? GG_USER_BLOCKED
			: GG_USER_NORMAL;
}

void GaduProtocol::sendUserList()
{
	kdebugf();

	UinType *uins;
	char *types;

	QList<Contact> contacts = ContactManager::instance()->contacts(account());

	if (contacts.isEmpty())
	{
		gg_notify_ex(GaduSession, 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	int count = contacts.count();

	uins = new UinType[count];
	types = new char[count];

	int i = 0;

	foreach (const Contact &contact, contacts)
	{
		uins[i] = uin(contact);
		types[i] = notifyTypeFromContact(contact);
		++i;
	}

	gg_notify_ex(GaduSession, uins, types, count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;

	kdebugf2();
}

void GaduProtocol::socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin));
	Buddy buddy = contact.ownerBuddy();

	if (buddy.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		emit userStatusChangeIgnored(buddy);
		gg_remove_notify(GaduSession, uin);
		return;
	}

	contact.setAddress(ip);
	contact.setPort(port);
	contact.setProtocolVersion(QString::number(version));

	GaduContactDetails *details = gaduContactDetails(contact);
	if (details)
	{
		details->setMaxImageSize(maxImageSize);
		details->setGaduProtocolVersion(version);
	}

	Status oldStatus = contact.currentStatus();
	Status newStatus;
	newStatus.setType(statusTypeFromGaduStatus(status));
	newStatus.setDescription(description);
	contact.setCurrentStatus(newStatus);

	emit contactStatusChanged(contact, oldStatus);
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
			MessageDialog::msg(msg, false, "Warning");
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
			MessageDialog::msg(tr("Connection will be stopped\nYour password is incorrect!"), false, "Critical");
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			tryAgain = false;
			MessageDialog::msg(tr("Connection will be stopped\nToo many attempts with bad password"), false, "Critical");
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

unsigned int GaduProtocol::uin(Contact contact) const
{
	GaduContactDetails *data = gaduContactDetails(contact);
	return data
			? data->uin()
			: 0;
}

GaduContactDetails * GaduProtocol::gaduContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<GaduContactDetails *>(contact.details());
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

void GaduProtocol::contactAdded(Contact contact)
{
	if (contact.contactAccount() != account())
		return;

	GaduContactDetails *details = gaduContactDetails(contact);
	if (!details)
		return;

	gg_add_notify_ex(GaduSession, details->uin(), notifyTypeFromContact(contact));
}

void GaduProtocol::contactAboutToBeRemoved(Contact contact)
{
	if (contact.contactAccount() != account())
		return;

	GaduContactDetails *details = gaduContactDetails(contact);
	if (!details)
		return;

//	TODO: 0.6.6 which one is the *right* way?
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_NORMAL);
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_BLOCKED);
// 	gg_remove_notify_ex(GaduSession, gcad->uin(), GG_USER_OFFLINE);
	gg_remove_notify(GaduSession, details->uin());
}

void GaduProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
	if (contact.contactAccount() != account())
		return;

	bool ok;
	UinType oldUin = oldId.toInt(&ok);
	if (ok)
		gg_remove_notify(GaduSession, oldUin);

	UinType newUin = uin(contact);
	if (newUin)
		gg_add_notify_ex(GaduSession, newUin, notifyTypeFromContact(contact));
}
