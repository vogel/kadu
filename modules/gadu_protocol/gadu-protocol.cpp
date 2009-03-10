 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#include <arpa/inet.h>

#include "accounts/account.h"

#include "chat/message/message.h"

#include "conference/conference-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/ignored-helper.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"

#include "dcc/dcc-manager.h"
#include "helpers/gadu-formatter.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "connection-timeout-timer.h"
#include "gadu-account.h"
#include "gadu-contact-account-data.h"
#include "gadu_images_manager.h"

#include "gadu-protocol.h"

void GaduProtocol::initModule()
{
	kdebugf();

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

GaduProtocol::GaduProtocol(Account *account, ProtocolFactory *factory) :
		Protocol(account, factory), Dcc(0),
		ActiveServer(), GaduLoginParams(), GaduSession(0),
		DccExternalIP(), PingTimer(0)
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

	disconnectedSlot();
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

int GaduProtocol::statusToType(Status status)
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

Status GaduProtocol::typeToStatus(int type)
{
	switch (type)
	{
		case GG_STATUS_AVAIL:
		case GG_STATUS_AVAIL_DESCR:
			return Status::Online;
		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			return Status::Busy;
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE_DESCR:
			return Status::Invisible;
		default:
			return Status::Offline;
	}
}

void GaduProtocol::changeStatus(Status newStatus)
{
	if (newStatus.isOffline() && status().isOffline())
	{
		if (NetworkConnecting == state())
			disconnectedSlot();
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
	int friends = (!newStatus.isOffline() && false /*newStatus.isFriendsOnly()*/ ? GG_STATUS_FRIENDS_MASK : 0);
	int type = statusToType(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, unicode2cp(newStatus.description()));
	else
		gg_change_status(GaduSession, type | friends);

	if (newStatus.isOffline())
		disconnectedSlot();

	statusChanged(newStatus);
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

QHostAddress GaduProtocol::activeServer()
{
	return ActiveServer;
}

void GaduProtocol::setDccExternalIP(const QHostAddress &ip)
{
	DccExternalIP = ip;
}

void GaduProtocol::disconnectedSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

//	socket notifiers are not watching any more
// 	SocketNotifiers->watchFor(0); // stop watching

	if (GaduSession)
	{
// we are disconnected, we cannot logoff
// 		gg_logoff(GaduSession);
		gg_free_session(GaduSession);
		GaduSession = 0;
	}

	// du�o bezsensownej roboty, wi�c gdy jeste�my w trakcie wy��czania,
	// to jej nie wykonujemy
	// dla ka�dego kontaktu po ustawieniu statusu emitowane s� sygna�y,
	// kt�re powoduj� od�wie�enie panelu informacyjnego, zapisanie status�w,
	// od�wie�enie okien chat�w, od�wie�enie userboksa
	if (!Kadu::closing())
		setAllOffline();

	if (!status().isOffline() && !nextStatus().isOffline())
		setStatus(Status::Offline);

	networkDisconnected();
	kdebugf2();
}

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

	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(this, SLOT(connectionTimeoutTimerSlot()));

	GaduSession = gg_login(&GaduLoginParams);

	cleanUpLoginParams();

	if (GaduSession)
		SocketNotifiers->watchFor(GaduSession);
	else
	{
		setStatus(Status::Offline);
		disconnectedSlot();
		emit error(Disconnected);
	}

	kdebugf2();
}

void GaduProtocol::setupProxy()
{
	kdebugf();

	if (gg_proxy_host)
	{
		free(gg_proxy_host);
		gg_proxy_host = NULL;
	}

	if (gg_proxy_username)
	{
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = NULL;
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
	if (Dcc) // TODO 0.6.6: do not recreate on each login attempt
	{
		delete Dcc;
		Dcc = 0;
	}

	if (config_file.readBoolEntry("Network", "AllowDCC"))
		Dcc = new DccManager(this);
}

void GaduProtocol::setupLoginParams()
{
	memset(&GaduLoginParams, 0, sizeof(GaduLoginParams));

	GaduAccount *gaduAccount = dynamic_cast<GaduAccount *>(account());
	GaduLoginParams.uin = gaduAccount->id().toULong();
	GaduLoginParams.password = strdup(gaduAccount->password().toAscii().data());

	GaduLoginParams.async = 1;
	GaduLoginParams.status = statusToType(nextStatus()); // TODO: 0.6.6 support is friend only
	if (!nextStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup((const char *)unicode2cp(nextStatus().description()).data());

	ActiveServer = GaduServersManager::instance()->getGoodServer();
	bool haveServer = !ActiveServer.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? GaduServersManager::instance()->getGoodPort() : 0;

	GaduLoginParams.protocol_version = 0x2a; // we are gg 7.7 now
	GaduLoginParams.client_version = "7, 7, 0, 3351";

	GaduLoginParams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	GaduLoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	bool haveDcc = config_file.readBoolEntry("Network", "AllowDCC") && DccExternalIP.toIPv4Address() &&
			config_file.readNumEntry("Network", "ExternalPort") > 1023;
	GaduLoginParams.external_addr = haveDcc ? htonl(DccExternalIP.toIPv4Address()) : 0;
	GaduLoginParams.external_port = haveDcc ? config_file.readNumEntry("Network", "ExternalPort") : 0;

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

void GaduProtocol::networkDisconnected()
{
	networkStateChanged(NetworkDisconnected);

	if (Dcc)
	{
		delete Dcc;
		Dcc = 0;
	}
}

void GaduProtocol::sendUserListLater()
{
	QTimer::singleShot(0, this, SLOT(sendUserList()));
}

void GaduProtocol::sendUserList()
{
	kdebugf();
	UinType *uins;
	char *types;

	ContactList contacts = ContactManager::instance()->contacts(account());

	if (contacts.isEmpty())
	{
		gg_notify_ex(GaduSession, NULL, NULL, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = new UinType[contacts.count()];
	types = new char[contacts.count()];

	int i = 0;

	foreach (Contact contact, contacts)
	{
		uins[i] = uin(contact);

		if (contact.isOfflineTo(account()))
			types[i] = GG_USER_OFFLINE;
		else
			if (contact.isBlocked(account()))
				types[i] = GG_USER_BLOCKED;
			else
				types[i] = GG_USER_NORMAL;
		++i;
	}

	gg_notify_ex(GaduSession, uins, types, contacts.count());
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;

	kdebugf2();
}

/* informacje osobiste */
void GaduProtocol::getPersonalInfo(SearchRecord &searchRecord)
{
	kdebugf();

	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_READ);
	searchRecord.Seq = gg_pubdir50(GaduSession, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::setPersonalInfo(SearchRecord &searchRecord, SearchResult &newData)
{
	kdebugf();

	gg_pubdir50_t req;
	req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

	if (!newData.First.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)(unicode2cp(newData.First).data()));
	if (!newData.Last.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)(unicode2cp(newData.Last).data()));
	if (!newData.Nick.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)(unicode2cp(newData.Nick).data()));
	if (!newData.City.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)(unicode2cp(newData.City).data()));
	if (!newData.Born.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)(unicode2cp(newData.Born).data()));
	if (newData.Gender)
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(newData.Gender).toLatin1());
	if (!newData.FamilyName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(newData.FamilyName).data()));
	if (!newData.FamilyCity.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)(unicode2cp(newData.FamilyCity).data()));

	searchRecord.Seq = gg_pubdir50(GaduSession, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::connectAfterOneSecond()
{
	kdebugf();
	QTimer::singleShot(1000, this, SLOT(login()));
	kdebugf2();
}

void GaduProtocol::socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin));

	if (contact.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
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
	Status newStatus = typeToStatus(status);
	newStatus.setDescription(description);
	accountData->setStatus(newStatus);

	emit contactStatusChanged(account(), contact, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
	kdebugf();
	QString msg = QString::null;

	disconnectedSlot();

// 	emit error(err);

	bool continue_connecting = true;
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
			continue_connecting = false;
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
			continue_connecting = false;
			MessageBox::msg(tr("Connection will be stopped\nYour password is incorrect!"), false, "Critical");
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			continue_connecting = false;
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

	if (!continue_connecting)
		setStatus(Status::Offline);

	// je�li b��d kt�ry wyst�pi� umo�liwia dalsze pr�by po��czenia
	// i w mi�dzyczasie u�ytkownik nie zmieni� statusu na niedost�pny
	// to za sekund� pr�bujemy ponownie
	if (continue_connecting && !nextStatus().isOffline())
		connectAfterOneSecond();

	kdebugf2();
}

void GaduProtocol::socketConnSuccess()
{
	kdebugf();

	ConnectionTimeoutTimer::off();

	sendUserList();

	GaduServersManager::instance()->markServerAsGood(QHostAddress(ntohl(GaduSession->server_addr)));
	GaduServersManager::instance()->markPortAsGood(GaduSession->port);

	/* jezeli sie rozlaczymy albo stracimy polaczenie, proces laczenia sie z serwerami zaczyna sie od poczatku */
	PingTimer = new QTimer(0);
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	statusChanged(nextStatus());
	networkConnected();

	// po po��czeniu z sewerem niestety trzeba ponownie ustawi�
	// status, inaczej nie b�dziemy widoczni - raczej b��d serwer�w
	if (status().isInvisible()
// TODO: 0.6.6
//|| (GaduLoginParams.status&~GG_STATUS_FRIENDS_MASK) != static_cast<GaduStatus *>(NextStatus)->toStatusNumber())
		)
		setStatus(status());

	kdebugf2();
}

void GaduProtocol::socketDisconnected()
{
	kdebugf();

	setStatus(Status::Offline);
	disconnectedSlot();

	kdebugf2();
}

#define GG_STATUS_INVISIBLE2 0x0009
Status::StatusType GaduProtocol::statusTypeFromIndex(unsigned int index) const
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

void GaduProtocol::userStatusChanged(const struct gg_event *e)
{
	kdebugf();

	int gadu_status_id;
	QString description;

	uint32_t uin;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint8_t version;
	uint8_t image_size;

	if (e->type == GG_EVENT_STATUS60)
	{
		uin = e->event.status60.uin;
		gadu_status_id = e->event.status60.status;
		description = cp2unicode(e->event.status60.descr);
		remote_ip = e->event.status60.remote_ip;
		remote_port = e->event.status60.remote_port;
		version = e->event.status60.version;
		image_size = e->event.status60.image_size;
	}
	else
	{
		uin = e->event.status.uin;
		gadu_status_id = e->event.status.status;
		description = cp2unicode(e->event.status.descr);
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	description.replace("\r\n", "\n");
	description.replace("\r", "\n");

	Status status(statusTypeFromIndex(gadu_status_id), description);
// TODO: 0.6.6
//	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went %d (%s)\n", uin,
//		status.type(), qPrintable(status.name()));

	Contact contact = account()->getContactById(QString::number(uin));

	if (contact.isAnonymous())
	{
		// ignore!
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(GaduSession, uin);
		emit userStatusChangeIgnored(contact);
		return;
	}

	GaduContactAccountData *data = gaduContactAccountData(contact);

	if (status.isOffline())
	{
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	if (!data)
		return;

	data->setIp(QHostAddress((quint32)(ntohl(remote_ip))));
	data->setPort(remote_port);
	data->setProtocolVersion(QString::number(version));
	data->setGaduProtocolVersion(version);
	data->setMaxImageSize(image_size);

// TODO: 0.6.5
// 	user.refreshDNSName("Gadu");

	Status oldStatus = data->status();
	data->setStatus(status);

	emit contactStatusChanged(account(), contact, oldStatus);
}

void GaduProtocol::setDccIpAndPort(unsigned long dcc_ip, int dcc_port)
{
	gg_dcc_ip = dcc_ip;
	gg_dcc_port = dcc_port;
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
	QString description = status.description().isNull()
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


