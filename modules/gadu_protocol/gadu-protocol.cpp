 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#ifdef Q_OS_WIN
#include <winsock2.h>
// name conflict..
#undef MessageBox
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"

#include "chat/message/message.h"

#include "conference/conference-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/ignored-helper.h"

#include "protocols/protocols_manager.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc/misc.h"

#include "dcc/dcc-manager.h"
#include "helpers/gadu-formatter.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "gadu-account.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol-factory.h"

#include "gadu-protocol.h"

extern "C" int gadu_protocol_init(bool firstLoad)
{
	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	ProtocolsManager::instance()->registerProtocolFactory("gadu", new GaduProtocolFactory());

	if (!xml_config_file->hasNode("Accounts"))
		GaduImporter::instance()->importAccounts();

	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" void gadu_protocol_close()
{
	ProtocolsManager::instance()->unregisterProtocolFactory("gadu");
}

void GaduProtocol::initModule()
{
	kdebugf();

	gg_debug_level = debug_mask | ~255;

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

#ifndef DEBUG_ENABLED
	gg_debug_level = 1;
#endif

// TODO: 0.6.6
//	defaultdescriptions = QStringList::split("<-->", config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);

	kdebugf2();
}

#define GG_STATUS_INVISIBLE2 0x0009
Status::StatusType GaduProtocol::statusTypeFromGaduStatus(unsigned int index)
{
	switch (index)
	{
		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return Status::Online;

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return Status::Busy;

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return Status::Invisible;

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:
		default:
			return Status::Offline;
	}

	return Status::Offline;
}

unsigned int GaduProtocol::gaduStatusFromStatus(const Status &status)
{
	bool hasDescription = !status.description().isEmpty();
	switch (status.type())
	{
		case Status::Online:
			return hasDescription ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;
		case Status::Busy:
			return hasDescription ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;
		case Status::Invisible:
			return hasDescription ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;
			break;
		default:
			return hasDescription ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
	}
}

GaduProtocol::GaduProtocol(Account *account, ProtocolFactory *factory) :
		Protocol(account, factory), Dcc(0),
		ActiveServer(), GaduLoginParams(), GaduSession(0), PingTimer(0)
{
	kdebugf();

	SocketNotifiers = new GaduProtocolSocketNotifiers(account, this);

	CurrentChatImageService = new GaduChatImageService(this);
	CurrentChatService = new GaduChatService(this);
	CurrentContactListService = new GaduContactListService(this);
	CurrentFileTransferService = new GaduFileTransferService(this);
	CurrentPersonalInfoService = new GaduPersonalInfoService(this);
	CurrentSearchService = new GaduSearchService(this);

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	networkDisconnected(false);
	delete SocketNotifiers;

	kdebugf2();
}

bool GaduProtocol::validateUserID(QString &uid)
{
	return true;
/*
	QIntValidator v(1, 99999999, this);
	int pos = 0;

	if ((uid != id) && (v.validate(uid, pos) == QValidator::Acceptable))
		return true;

	return false;
*/
}

unsigned int GaduProtocol::maxDescriptionLength()
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

	if (newStatus.isOffline() && status().isOffline())
	{
		if (NetworkConnecting == state())
			networkDisconnected(false);
		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isOffline())
	{
		login();
		return;
	}

// TODO: 0.6.6
	int friends = (!newStatus.isOffline() && privateMode() ? GG_STATUS_FRIENDS_MASK : 0);
	int type = gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, unicode2cp(newStatus.description()));
	else
		gg_change_status(GaduSession, type | friends);

	if (newStatus.isOffline())
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

void GaduProtocol::login()
{
	kdebugf();

	GaduAccount *gaduAccount = dynamic_cast<GaduAccount *>(account());

	if (0 == gaduAccount->uin() || QString::null == gaduAccount->password())
	{
		MessageBox::msg(tr("UIN or password not set!"), false, "Warning");
		setStatus(Status::Offline);
		kdebugmf(KDEBUG_FUNCTION_END, "end: uin or password not set\n");
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

	gg_proxy_enabled = config_file.readBoolEntry("Network", "UseProxy");

	if (gg_proxy_enabled)
	{
		gg_proxy_host = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyHost")).data());
		gg_proxy_port = config_file.readNumEntry("Network", "ProxyPort");

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

		if (!config_file.readEntry("Network", "ProxyUser").isEmpty())
		{
			gg_proxy_username = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyUser")).data());
			gg_proxy_password = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyPassword")).data());
		}
	}

	kdebugf2();
}

void GaduProtocol::setupDcc()
{
	GaduAccount *gaduAccount = dynamic_cast<GaduAccount *>(account());
	if (!gaduAccount)
		return;
	if (gaduAccount->allowDCC())
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

	GaduAccount *gaduAccount = dynamic_cast<GaduAccount *>(account());
	if (!gaduAccount)
		return;
	GaduLoginParams.uin = gaduAccount->id().toULong();
	GaduLoginParams.password = strdup(gaduAccount->password().toAscii().data());

	GaduLoginParams.async = 1;
	GaduLoginParams.status = gaduStatusFromStatus(nextStatus()); // TODO: 0.6.6 support is friend only
	if (!nextStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup((const char *)unicode2cp(nextStatus().description()).data());

	ActiveServer = GaduServersManager::instance()->getServer();
	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	GaduLoginParams.protocol_version = 0x2a; // we are gg 7.7 now
	GaduLoginParams.client_version = "7, 7, 0, 3351";

	GaduLoginParams.has_audio = gaduAccount->allowDCC();
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

	if (tryAgain && !nextStatus().isOffline()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	else if (!nextStatus().isOffline())
		setStatus(Status::Offline);
}

void GaduProtocol::sendUserList()
{
	kdebugf();

	UinType *uins;
	char *types;

	ContactList contacts = ContactManager::instance()->contacts(account());

	if (contacts.isEmpty())
	{
		gg_notify_ex(GaduSession, 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = new UinType[contacts.count()];
	types = new char[contacts.count()];

	int i = 0;

	foreach (Contact contact, contacts)
	{
		uins[i] = uin(contact);
		types[i] = contact.isOfflineTo(account())
			? GG_USER_OFFLINE
			: contact.isBlocked(account())
				? GG_USER_BLOCKED
				: GG_USER_NORMAL;
		++i;
	}

	gg_notify_ex(GaduSession, uins, types, contacts.count());
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;

	kdebugf2();
}

void GaduProtocol::socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin));

	if (contact.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		emit userStatusChangeIgnored(contact);
		gg_remove_notify(GaduSession, uin);
		return;
	}

	GaduContactAccountData *accountData = gaduContactAccountData(contact);
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

	emit contactStatusChanged(account(), contact, oldStatus);
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
			break;
	}
/*
	if (msg != QString::null)
	{
		QHostAddress server = activeServer();
		QString host;
		if (!server.isNull())
			host = server.toString();
		else
			host = "HUB";
		kdebugm(KDEBUG_INFO, "%s %s\n", qPrintable(host), qPrintable(msg));
		emit connectionError(account(), host, msg);
	}*/

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
	if (status().isInvisible())
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
	GaduContactAccountData *data = gaduContactAccountData(contact);
	return data
		? data->uin()
		: 0;
}

GaduContactAccountData * GaduProtocol::gaduContactAccountData(Contact contact) const
{
	return dynamic_cast<GaduContactAccountData *>(contact.accountData(account()));
}

QPixmap GaduProtocol::statusPixmap(Status status)
{
	QString description = status.description().isEmpty()
			? ""
			: "WithDescription";
	QString pixmapName;

	switch (status.type())
	{
		case Status::Online:
			pixmapName = QString("Online").append(description);
			break;
		case Status::Busy:
			pixmapName = QString("Busy").append(description);
			break;
		case Status::Invisible:
			pixmapName = QString("Invisible").append(description);
			break;
		default:
			pixmapName = QString("Offline").append(description);
			break;
	}

	return icons_manager->loadPixmap(pixmapName);
}

GaduConference * GaduProtocol::conference(ContactList contacts)
{
	foreach (Conference *c, ConferenceManager::instance()->conferences())
	{
		if (c->account() != account())
			continue;
		GaduConference *gc = dynamic_cast<GaduConference *>(c);
		if (!gc)
			continue;
		if (gc->contacts() == contacts)
			return gc;
	}

	GaduConference *conference = new GaduConference(account(), contacts);
	ConferenceManager::instance()->addConference(conference);
	return conference;
}

Conference * GaduProtocol::loadConferenceFromStorage(StoragePoint *storage)
{
	Conference *result = new Conference(account(), QUuid());
	result->setStorage(storage);
	result->loadConfiguration();
	return result;
}
