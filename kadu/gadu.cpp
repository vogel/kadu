/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "gadu-private.h"
#include "gadu_images_manager.h"
#include "gadu_rich_text.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu-config.h"
#include "message_box.h"
#include "misc.h"

//netinet/in.h na freebsd 4.x jest wybrakowane i trzeba inkludowaæ sys/types.h
#include <sys/types.h>
//dla htonl
#include <netinet/in.h>

#include <stdlib.h>

#include <qcheckbox.h>
#include <qtimer.h>
#include <qvgroupbox.h>

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

static QValueList<QHostAddress> gg_servers;

#define GG_SERVERS_COUNT 12
static const char *gg_servers_ip[GG_SERVERS_COUNT] = {
	"217.17.41.82",
	"217.17.41.83",
	"217.17.41.84",
	"217.17.41.85",
	"217.17.41.86",
	"217.17.41.87",
	"217.17.41.88",
	"217.17.41.92",
	"217.17.41.93",
	"217.17.45.133",
	"217.17.45.143",
	"217.17.45.144"
};

// ------------------------------------
//              Timers
// ------------------------------------

class ConnectionTimeoutTimer : public QTimer {
	public:
		static void on();
		static void off();
		static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

	private:
		ConnectionTimeoutTimer(QObject *parent = 0, const char *name=0);

		static ConnectionTimeoutTimer *connectiontimeout_object;
};

ConnectionTimeoutTimer *ConnectionTimeoutTimer::connectiontimeout_object = NULL;

// ------------------------------------
//        Timers - implementation
// ------------------------------------

ConnectionTimeoutTimer::ConnectionTimeoutTimer(QObject *parent, const char *name) : QTimer(parent, name)
{
	start(config_file.readUnsignedNumEntry("Network", "TimeoutInMs"), true);
}

bool ConnectionTimeoutTimer::connectTimeoutRoutine(const QObject *receiver, const char *member)
{
	return connect(connectiontimeout_object, SIGNAL(timeout()), receiver, member);
}

void ConnectionTimeoutTimer::on()
{
	if (!connectiontimeout_object)
		connectiontimeout_object = new ConnectionTimeoutTimer(kadu, "connection_timeout_timer_object");
}

void ConnectionTimeoutTimer::off()
{
	if (connectiontimeout_object)
	{
		delete connectiontimeout_object;
		connectiontimeout_object = NULL;
	}
}

// ------------------------------------
//              UinsList
// ------------------------------------

bool UinsList::equals(const UinsList &uins) const
{
	if (count() != uins.count())
		return false;
	CONST_FOREACH(i, *this)
		if(!uins.contains(*i))
			return false;
	return true;
}

UinsList::UinsList()
{
}

UinsList::UinsList(UinType uin)
{
	append(uin);
}

UinsList::UinsList (const QString &uins)
{
	QStringList list = QStringList::split (",", uins);
	CONST_FOREACH(it, list)
		append ((*it).toUInt ());
}

UinsList::UinsList (const QStringList &list)
{
	CONST_FOREACH(it, list)
		append ((*it).toUInt ());
}

void UinsList::sort()
{
	qHeapSort(*this);
}

QStringList UinsList::toStringList() const
{
	QStringList list;
	CONST_FOREACH(uin, *this)
		list.append(QString::number(*uin));
	return list;
}

SearchResult::SearchResult()
{
}

SearchResult::SearchResult(const SearchResult& copyFrom)
{
	Uin = copyFrom.Uin;
	First = copyFrom.First;
	Last = copyFrom.Last;
	Nick = copyFrom.Nick;
	Born = copyFrom.Born;
	City = copyFrom.City;
	FamilyName = copyFrom.FamilyName;
	FamilyCity = copyFrom.FamilyCity;
	Gender = copyFrom.Gender;
	Stat = copyFrom.Stat;
}

void SearchResult::setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
	const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status)
{
	kdebugf();
	Uin = cp2unicode((unsigned char *)uin);
	First = cp2unicode((unsigned char *)first);
	Last = cp2unicode((unsigned char *)last);
	Nick = cp2unicode((unsigned char *)nick);
	Born = cp2unicode((unsigned char *)born);
	City = cp2unicode((unsigned char *)city);
	FamilyName = cp2unicode((unsigned char *)familyName);
	FamilyCity = cp2unicode((unsigned char *)familyCity);
	if (status)
		Stat.fromStatusNumber(atoi(status) & 127, QString::null);
	if (gender)
		Gender = atoi(gender);
	else
		Gender = 0;
	kdebugf2();
}

SearchRecord::SearchRecord()
{
	kdebugf();
	clearData();
	kdebugf2();
}

SearchRecord::~SearchRecord()
{
}

void SearchRecord::reqUin(const QString& uin)
{
	Uin = uin;
}

void SearchRecord::reqFirstName(const QString& firstName)
{
	FirstName = firstName;
}

void SearchRecord::reqLastName(const QString& lastName) {
	LastName = lastName;
}

void SearchRecord::reqNickName(const QString& nickName)
{
	NickName = nickName;
}

void SearchRecord::reqCity(const QString& city)
{
	City = city;
}

void SearchRecord::reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void SearchRecord::reqGender(bool female)
{
	Gender = (female ? 2 : 1);
}

void SearchRecord::reqActive()
{
	Active = true;
}

void SearchRecord::clearData()
{
	kdebugf();
	FromUin = 0;
	Uin.truncate(0);
	FirstName.truncate(0);
	LastName.truncate(0);
	NickName.truncate(0);
	City.truncate(0);
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Gender = 0;
	Active = false;
	kdebugf2();
}

/* GaduProtocol */

QValueList<QHostAddress> GaduProtocol::ConfigServers;
static GaduProtocolManager *gadu_protocol_manager;

void GaduProtocol::closeModule()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Network", "Use default servers", SIGNAL(toggled(bool)),
		gadu, SLOT(ifDefServerEnabled(bool)));

	ConfigDialog::unregisterSlotOnCreateTab("Network", gadu, SLOT(onCreateTabNetwork()));
	ConfigDialog::unregisterSlotOnApplyTab("Network", gadu, SLOT(onApplyTabNetwork()));

	ConfigDialog::removeControl("Network", "Port to connect to servers");
	ConfigDialog::removeControl("Network", "IP addresses:", "server");
#ifdef HAVE_OPENSSL
	ConfigDialog::removeControl("Network", "Use TLSv1");
#endif

	ConfigDialog::removeControl("Network", "Use default servers");
	ConfigDialog::removeControl("Network", "servergrid");
	ConfigDialog::removeControl("Network", "Servers properties");

	ConfigDialog::removeControl("Network", " Password: ");
	ConfigDialog::removeControl("Network", "Username: ");
	ConfigDialog::removeControl("Network", " Port: ");
	ConfigDialog::removeControl("Network", "Host: ", "proxyhost");
	ConfigDialog::removeControl("Network", "proxygrid");
	ConfigDialog::removeControl("Network", "Proxy server");

	ConfigDialog::removeControl("Network", "Use proxy server");

	ConfigDialog::removeControl("Network", "dcc");

	protocols_manager->unregisterProtocol("Gadu");
	delete gadu_protocol_manager;
	gadu_protocol_manager = NULL;
	delete gadu;
	gadu = NULL;
	kdebugf2();
}

void GaduProtocol::changeID(const QString &newID)
{
	if (id != newID)
		id = newID;
}

void GaduProtocol::initModule()
{
	kdebugf();
	gadu_protocol_manager = new GaduProtocolManager();
	protocols_manager->registerProtocol("Gadu", "Gadu-Gadu", gadu_protocol_manager);

	gadu = static_cast<GaduProtocol *>(protocols_manager->newProtocol("Gadu", QString::number(config_file.readNumEntry("General", "UIN"))));
//	gadu = new GaduProtocol(QString::number(config_file.readNumEntry("General", "UIN")), kadu, "gadu");

	QHostAddress ip;
	for (int i = 0; i < GG_SERVERS_COUNT; ++i)
	{
		ip.setAddress(QString(gg_servers_ip[i]));
		gg_servers.append(ip);
	}

	gg_proxy_host = NULL;
	gg_proxy_username = NULL;
	gg_proxy_password = NULL;


	// ---------------------------------------------------------
	//  Okno dialogowe - czê¶æ kodu przejdzie do klasy Protocol
	// ---------------------------------------------------------
//zakladka "siec"
	ConfigDialog::addVBox("Network", "Network", "dcc");

	ConfigDialog::addCheckBox("Network", "Network",
		QT_TRANSLATE_NOOP("@default", "Use proxy server"), "UseProxy", false, 0, 0, Advanced);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Proxy server"), 0, Advanced);
	ConfigDialog::addGrid("Network", "Proxy server", "proxygrid", Expert);
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Host: "), "ProxyHost", "0.0.0.0", 0, "proxyhost");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", " Port: "), "ProxyPort", "0");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", "Username: "), "ProxyUser");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", " Password: "), "ProxyPassword");

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Servers properties"), 0, Expert);
	ConfigDialog::addGrid("Network", "Servers properties", "servergrid", Expert);
	ConfigDialog::addCheckBox("Network", "servergrid",
		QT_TRANSLATE_NOOP("@default", "Use default servers"), "isDefServers", true);
#ifdef HAVE_OPENSSL
	ConfigDialog::addCheckBox("Network", "servergrid",
		QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
	ConfigDialog::addLineEdit("Network", "Servers properties",
		QT_TRANSLATE_NOOP("@default", "IP addresses:"), "Server", 0, 0, "server");

	config_file.addVariable("Network", "DefaultPort", 0);
	QStringList options, values;
	options << tr("Automatic") << "8074" << "443";
	values << "0" << "8074" << "443";
	ConfigDialog::addComboBox("Network", "Servers properties",
		QT_TRANSLATE_NOOP("@default", "Port to connect to servers"), "DefaultPort", options, values);

	config_file.addVariable("Network", "TimeoutInMs", 5000);

	ConfigDialog::registerSlotOnCreateTab("Network", gadu, SLOT(onCreateTabNetwork()));
	ConfigDialog::registerSlotOnApplyTab("Network", gadu, SLOT(onApplyTabNetwork()));

	ConfigDialog::connectSlot("Network", "Use default servers", SIGNAL(toggled(bool)),
		gadu, SLOT(ifDefServerEnabled(bool)));
#ifdef HAVE_OPENSSL
	ConfigDialog::connectSlot("Network", "Use TLSv1", SIGNAL(toggled(bool)),
		gadu, SLOT(useTlsEnabled(bool)));
#endif

	defaultdescriptions = QStringList::split("<-->", config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);

	QStringList servers;
	QHostAddress ip2;
	servers = QStringList::split(";", config_file.readEntry("Network", "Server"));
	ConfigServers.clear();
	CONST_FOREACH(server, servers)
		if (ip2.setAddress(*server))
			ConfigServers.append(ip2);

	kdebugf2();
}

GaduProtocol::GaduProtocol(const QString &id, QObject *parent, const char *name) : Protocol(id, parent, name)
{
	kdebugf();

	ProtocolID = "Gadu";

	SendUserListTimer = new QTimer(this, "SendUserListTimer");
	whileConnecting = false;
	Sess = NULL;
	CurrentStatus = new GaduStatus();
	NextStatus = new GaduStatus();
	SocketNotifiers = new GaduSocketNotifiers(this, "gadu_socket_notifiers");
	PingTimer = NULL;
	ActiveServer = NULL;
	ServerNr = 0;

	connect(NextStatus, SIGNAL(goOnline(const QString &)), this, SLOT(iWantGoOnline(const QString &)));
	connect(NextStatus, SIGNAL(goBusy(const QString &)), this, SLOT(iWantGoBusy(const QString &)));
	connect(NextStatus, SIGNAL(goInvisible(const QString &)), this, SLOT(iWantGoInvisible(const QString &)));
	connect(NextStatus, SIGNAL(goOffline(const QString &)), this, SLOT(iWantGoOffline(const QString &)));

	connect(SocketNotifiers, SIGNAL(ackReceived(int, uin_t, int)), this, SLOT(ackReceived(int, uin_t, int)));
	connect(SocketNotifiers, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(SocketNotifiers, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SIGNAL(dccConnectionReceived(const UserListElement&)));
	connect(SocketNotifiers, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
	connect(SocketNotifiers, SIGNAL(error(GaduError)), this, SLOT(errorSlot(GaduError)));
	connect(SocketNotifiers, SIGNAL(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)),
		this, SLOT(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)));
	connect(SocketNotifiers, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SLOT(imageRequestReceivedSlot(UinType, uint32_t, uint32_t)));
	connect(SocketNotifiers, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)));
	connect(SocketNotifiers, SIGNAL(messageReceived(int, UserListElements, QCString &, time_t, QByteArray &)),
		this, SLOT(messageReceived(int, UserListElements, QCString &, time_t, QByteArray &)));
	connect(SocketNotifiers, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)), this, SLOT(newResults(gg_pubdir50_t)));
	connect(SocketNotifiers, SIGNAL(systemMessageReceived(QString &, QDateTime &, int, void *)),
		this, SLOT(systemMessageReceived(QString &, QDateTime &, int, void *)));
	connect(SocketNotifiers, SIGNAL(userlistReceived(const struct gg_event *)),
		this, SLOT(userListReceived(const struct gg_event *)));
	connect(SocketNotifiers, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
	connect(SocketNotifiers, SIGNAL(userStatusChanged(const struct gg_event *)),
		this, SLOT(userStatusChanged(const struct gg_event *)));

	connect(CurrentStatus, SIGNAL(changed(const UserStatus &, const UserStatus &)),
			this, SLOT(currentStatusChanged(const UserStatus &, const UserStatus &)));

	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAdded(UserListElement, QString, bool, bool)));
	connect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(removingProtocol(UserListElement, QString, bool, bool)));

	connect(SendUserListTimer, SIGNAL(timeout()), this, SLOT(sendUserList()));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnectedSlot();
	delete SocketNotifiers;

	kdebugf2();
}

UserStatus *GaduProtocol::newStatus() const
{
	return new GaduStatus();
}

void GaduProtocol::currentStatusChanged(const UserStatus &status, const UserStatus &oldStatus)
{
	if (userlist->contains("Gadu", QString::number(LoginParams.uin)))
		userlist->byID("Gadu", QString::number(LoginParams.uin)).setStatus("Gadu", *CurrentStatus);
}

void GaduProtocol::iWantGoOnline(const QString &desc)
{
	kdebugf();

	//nie pozwalamy na zmianê statusu lub ponowne logowanie gdy jeste¶my
	//w trakcie ³±czenia siê z serwerem, bo serwer zwróci nam g³upoty
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_AVAIL_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_AVAIL | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoBusy(const QString &desc)
{
	kdebugf();

	//patrz iWantGoOnline()
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_BUSY_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_BUSY | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoInvisible(const QString &desc)
{
	kdebugf();

	//patrz iWantGoOnline()
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_INVISIBLE_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_INVISIBLE | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoOffline(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline() && whileConnecting)
	{
		logout();//ustawia m.in. whileConnecting na false
		return;
	}

	if (CurrentStatus->isOffline())
		return;

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_NOT_AVAIL_DESCR, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_NOT_AVAIL);


	CurrentStatus->setStatus(*NextStatus);
	logout();

	kdebugf2();
}

void GaduProtocol::protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool /*last*/)
{
	kdebugf();
	/*
	   je¿eli listê kontaktów bêdziemy wysy³aæ po kawa³ku, to serwer zgubi czê¶æ danych!
	   musimy wiêc wys³aæ j± w ca³o¶ci (poprzez sendUserList())
	   w takim w³a¶nie przypadku (massively==true) nie robimy nic
	*/
	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;
	if (name != "OfflineTo" && name != "Blocking")
		return;

	if (massively)
	{
		sendUserListLater();
		return;
	}

	UinType uin = elem.ID("Gadu").toUInt();
	if (name == "OfflineTo")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_remove_notify_ex(Sess, uin, GG_USER_NORMAL);
			gg_add_notify_ex(Sess, uin, GG_USER_OFFLINE);
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_add_notify_ex(Sess, uin, GG_USER_OFFLINE);
			gg_add_notify_ex(Sess, uin, GG_USER_NORMAL);
		}
	}
	else if (name == "Blocking")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_remove_notify_ex(Sess, uin, GG_USER_NORMAL);
			gg_add_notify_ex(Sess, uin, GG_USER_BLOCKED);
			elem.setStatus(protocolName, GaduStatus());
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_remove_notify_ex(Sess, uin, GG_USER_BLOCKED);
			gg_add_notify_ex(Sess, uin, GG_USER_NORMAL);
		}
	}

	kdebugf2();
}

void GaduProtocol::userDataChanged(UserListElement elem, QString name, QVariant oldValue,
					QVariant currentValue, bool massively, bool /*last*/)
{
	kdebugf();
	if (!elem.usesProtocol("Gadu"))
		return;
	if (CurrentStatus->isOffline())
		return;
	if (name != "Anonymous")
		return;

	if (massively)
		sendUserListLater();
	else
	{
		if (name == "Anonymous")
			if (!currentValue.toBool() && oldValue.toBool())
				gg_add_notify(Sess, elem.ID("Gadu").toUInt());
	}
	kdebugf2();
}

void GaduProtocol::userAdded(UserListElement elem, bool massively, bool /*last*/)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", elem.altNick().local8Bit().data(), massively/*, last*/);
	if (!elem.usesProtocol("Gadu"))
		return;
	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!elem.isAnonymous())
			gg_add_notify(Sess, elem.ID("Gadu").toUInt());
	kdebugf2();
}

void GaduProtocol::removingUser(UserListElement elem, bool massively, bool /*last*/)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", elem.altNick().local8Bit().data(), massively/*, last*/);
	if (!elem.usesProtocol("Gadu"))
		return;
	if (CurrentStatus->isOffline())
		return;
	if (massively)
		sendUserListLater();
	else
		if (!elem.isAnonymous())
			gg_remove_notify(Sess, elem.ID("Gadu").toUInt());
	kdebugf2();
}

void GaduProtocol::protocolAdded(UserListElement elem, QString protocolName, bool massively, bool /*last*/)
{
	kdebugf();
	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!elem.isAnonymous())
			gg_add_notify(Sess, elem.ID("Gadu").toUInt());
	kdebugf2();
}

void GaduProtocol::removingProtocol(UserListElement elem, QString protocolName, bool massively, bool /*last*/)
{
	kdebugf();
	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!elem.isAnonymous())
			gg_remove_notify(Sess, elem.ID("Gadu").toUInt());
	kdebugf2();
}

QHostAddress* GaduProtocol::activeServer()
{
	return ActiveServer;
}

void GaduProtocol::setDccExternalIP(const QHostAddress& ip)
{
	DccExternalIP = ip;
}

void GaduProtocol::connectedSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();
	ConnectionTime = QDateTime::currentDateTime();

	whileConnecting = false;
	sendUserList();

	/* jezeli sie rozlaczymy albo stracimy polaczenie, proces laczenia sie z serwerami zaczyna sie od poczatku */
	ServerNr = 0;
	PingTimer = new QTimer(NULL, "PingTimer");
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(pingNetwork()));
	PingTimer->start(60000, TRUE);

	CurrentStatus->setStatus(*NextStatus);
	emit connected();

	// po po³±czeniu z sewerem niestety trzeba ponownie ustawiæ
	// status, inaczej nie bêdziemy widoczni - raczej b³±d serwerów
	if (NextStatus->isInvisible() || (LoginParams.status&~GG_STATUS_FRIENDS_MASK) != static_cast<GaduStatus *>(NextStatus)->toStatusNumber())
		NextStatus->refresh();

	/*
		UWAGA: je¿eli robimy refresh(), to przy przechodzeniu z niedostêpnego z opisem
		na niewidoczny z opisem ta zmiana jest ujawniana naszym kontaktom!
		przy przechodzeniu z niedostêpnego na niewidoczny efekt nie wystêpuje

		je¿eli NIE zrobimy refresh(), to powy¿szy efekt nie wystêpuje, ale przy
		przechodzeniu z niedostêpnego z opisem na niewidoczny (bez opisu), nasz
		opis u innych pozostaje! (a¿ do czasu naszej zmiany statusu lub ich
		roz³±czenia i po³±czenia)
	*/

	/*
		UWAGA 2: procedura ³±czenia siê z serwerem w chwili obecnej wykorzystuje
		fakt ponownego ustawienia statusu po zalogowaniu, bo iWantGo* blokuj±
		zmiany statusów w trakcie ³±czenia siê z serwerem
	*/

	kdebugf2();
}

void GaduProtocol::disconnectedSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = NULL;
	}

	SocketNotifiers->stop();

	if (Sess)
	{
		gg_logoff(Sess);
		gg_free_session(Sess);
		Sess = NULL;
	}

	// du¿o bezsensownej roboty, wiêc gdy jeste¶my w trakcie wy³±czania,
	// to jej nie wykonujemy
	// dla ka¿dego kontaktu po ustawieniu statusu emitowane s± sygna³y,
	// które powoduj± od¶wie¿enie panelu informacyjnego, zapisanie statusów,
	// od¶wie¿enie okien chatów, od¶wie¿enie userboksa
	if (!Kadu::closing())
		userlist->setAllOffline("Gadu");

	CurrentStatus->setOffline(QString::null);
	emit disconnected();
	kdebugf2();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();

	kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
	errorSlot(ConnectionTimeout);

	kdebugf2();
}

void GaduProtocol::errorSlot(GaduError err)
{
	kdebugf();
	QString msg = QString::null;

	ConnectionTimeoutTimer::off();

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = NULL;
	}

	SocketNotifiers->stop();

	if (Sess)
	{
		gg_logoff(Sess);
		gg_free_session(Sess);
		Sess = NULL;
	}

	emit error(err);

	bool continue_connecting = true;
	switch (err)
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
			MessageBox::msg(msg);
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
			MessageBox::wrn(tr("Connection will be stoped\nYour password is incorrect !"));
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;
			
		case ConnectionIntruderError:
			msg = tr("To many connection attempts with bad password!");
			continue_connecting = false;
			MessageBox::wrn(tr("Connection will be stoped\nTo many attempts with bad password"));
			break;
			
		case ConnectionUnavailableError:
			msg = tr("Unable to connect, servers are down");
			break;

		case ConnectionUnknow:
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");

			userlist->setAllOffline("Gadu");
			CurrentStatus->setOffline(QString::null);
			emit disconnected();
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occured");
			userlist->setAllOffline("Gadu");
			CurrentStatus->setOffline(QString::null);
			emit disconnected();
			break;

		default:
			kdebugm(KDEBUG_WARNING, "Unhandled error?\n");
			break;
	}

	if (msg != QString::null)
	{
		QHostAddress* server = activeServer();
		QString host;
		if (server != NULL)
			host = server->toString();
		else
			host = "HUB";
		msg = QString("(%1) %2").arg(host).arg(msg);
		kdebugm(KDEBUG_INFO, "%s\n", msg.local8Bit().data());
		emit connectionError(this, msg);
	}

	if (!continue_connecting)
	{
		whileConnecting = false;
		NextStatus->setOffline();
	}

	// je¶li b³±d który wyst±pi³ umo¿liwia dalsze próby po³±czenia
	// i w miêdzyczasie u¿ytkownik nie zmieni³ statusu na niedostêpny
	// to za sekundê próbujemy ponownie
	if (continue_connecting && !status().isOffline())
		connectAfterOneSecond();

	kdebugf2();
}

void GaduProtocol::imageReceived(UinType sender, uint32_t size, uint32_t crc32,
	const QString &filename, const char *data)
{
	kdebugm(KDEBUG_INFO, QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
		.arg(sender).arg(size).arg(crc32).arg(filename).local8Bit().data());

	QString full_path = gadu_images_manager.saveImage(sender,size,crc32,filename,data);
	emit imageReceivedAndSaved(sender, size, crc32, full_path);
}

void GaduProtocol::imageRequestReceivedSlot(UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugm(KDEBUG_INFO, QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(sender).arg(size).arg(crc32).local8Bit().data());

	gadu_images_manager.sendImage(sender,size,crc32);
}

void GaduProtocol::messageReceived(int msgclass, UserListElements senders, QCString &msg, time_t time,
	QByteArray &formats)
{
/*
	najpierw sprawdzamy czy nie jest to wiadomosc systemowa (senders[0] rowne 0)
	potem sprawdzamy czy user jest na naszej liscie, jezeli nie to anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/
	if (senders[0].ID("Gadu").toUInt() && senders[0].isAnonymous() && config_file.readBoolEntry("Chat","IgnoreAnonymousUsers"))
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", senders[0].ID("Gadu").toUInt());
		return;
	}

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders))
		return;

	bool block = false;
	emit messageFiltering(this, senders, msg, formats, block);
	if (block)
		return;

	const char* msg_c = msg;
	QString mesg = cp2unicode((const unsigned char*)msg_c);
	QDateTime datetime;
	datetime.setTime_t(time);

	bool grab = false;
	emit chatMsgReceived0(this, senders, mesg, time, grab);
	if (grab)
		return;

	// wiadomosci systemowe maja senders[0] = 0
	// FIX ME!!!
	if (senders[0].ID("Gadu").toUInt() == 0)
	{
		if (msgclass <= config_file.readNumEntry("General", "SystemMsgIndex", 0))
		{
			kdebugm(KDEBUG_INFO, "Already had this message, ignoring\n");
			return;
		}

		config_file.writeEntry("General", "SystemMsgIndex", msgclass);
		kdebugm(KDEBUG_INFO, "System message index %d\n", msgclass);

		emit systemMessageReceived(mesg, datetime, formats.size(), formats.data());
		return;
	}

	if (senders[0].isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext"))
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
	}
	else
		mesg = formatGGMessage(mesg, formats.size(), formats.data(), senders[0].ID("Gadu").toUInt());

	if (mesg.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			senders[0].ID("Gadu").toUInt(), (const char *)mesg.local8Bit());

	emit chatMsgReceived1(this, senders, mesg, time, grab);
	if (!grab)
		emit chatMsgReceived2(this, senders, mesg, time);
}

void GaduProtocol::pingNetwork()
{
	kdebugf();
	gg_ping(Sess);
	PingTimer->start(60000, TRUE);
	kdebugf2();
}

void GaduProtocol::systemMessageReceived(QString &message, QDateTime &time, int /*formats_length*/, void * /*formats*/)
{
	kdebugf();

	QString mesg = time.toString("hh:mm:ss (dd.MM.yyyy): ") + message;
	emit systemMessageReceived(mesg);

	kdebugf2();
}

void GaduProtocol::login()
{
	kdebugf();
	if (ID() == "0" || ID().isEmpty() || config_file.readEntry("General", "Password").isEmpty())
	{
		MessageBox::wrn(tr("UIN or password not set!"));
		NextStatus->setOffline();
		kdebugmf(KDEBUG_FUNCTION_END, "end: uin or password not set\n");
		return;
	}

	whileConnecting = true;

	emit connecting();

	memset(&LoginParams, 0, sizeof(LoginParams));
	LoginParams.async = 1;

	// maksymalny rozmiar grafiki w kb
	LoginParams.image_size = config_file.readNumEntry("Chat", "MaxImageSize", 20);

	setupProxy();

	LoginParams.status = static_cast<GaduStatus *>(NextStatus)->toStatusNumber();
	if (NextStatus->isFriendsOnly())
		LoginParams.status |= GG_STATUS_FRIENDS_MASK;
	if (NextStatus->hasDescription())
		LoginParams.status_descr = strdup((const char *)unicode2cp(NextStatus->description()).data());

	LoginParams.uin = (UinType) ID().toUInt();
	LoginParams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	// GG 6.0 build 147 ustawia indeks ostatnio odczytanej wiadomosci systemowej na 1389
	LoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	if (config_file.readBoolEntry("Network", "AllowDCC") && DccExternalIP.ip4Addr() && config_file.readNumEntry("Network", "ExternalPort") > 1023)
	{
		LoginParams.external_addr = htonl(DccExternalIP.ip4Addr());
		LoginParams.external_port = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		LoginParams.external_addr = 0;
		LoginParams.external_port = 0;
	}

	if (!ConfigServers.isEmpty() && !config_file.readBoolEntry("Network", "isDefServers") && ConfigServers[ServerNr].ip4Addr())
	{
		ActiveServer = &ConfigServers[ServerNr];
		++ServerNr;
		if (ServerNr >= ConfigServers.count())
			ServerNr = 0;
	}
	else
	{
		if (ServerNr)
			ActiveServer = &gg_servers[ServerNr - 1];
		else
			ActiveServer = NULL;
		++ServerNr;
		if (ServerNr > gg_servers.count())
			ServerNr = 0;
	}

	if (ActiveServer != NULL)
	{
		LoginParams.server_addr = htonl(ActiveServer->ip4Addr());
		LoginParams.server_port = config_file.readNumEntry("Network", "DefaultPort");
	}
	else
	{
		LoginParams.server_addr = 0;
		LoginParams.server_port = 0;
	}

//	polaczenia TLS z serwerami GG na razie nie dzialaja
//	LoginParams.tls = config_file.readBoolEntry("Network", "UseTLS");
	LoginParams.tls = 0;
	LoginParams.client_version = GG_DEFAULT_CLIENT_VERSION; //tego siê nie zwalnia...
	LoginParams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	if (LoginParams.tls)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "using TLS\n");
		LoginParams.server_port = 0;
		if (config_file.readBoolEntry("Network", "isDefServers"))
			LoginParams.server_addr = 0;
		LoginParams.server_port = 443;
	}
	else
		LoginParams.server_port = config_file.readNumEntry("Network", "DefaultPort");

	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(this, SLOT(connectionTimeoutTimerSlot()));

	LoginParams.password = strdup((const char *)unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	Sess = gg_login(&LoginParams);
	memset(LoginParams.password, 0, strlen(LoginParams.password));
	free(LoginParams.password);

	if (LoginParams.status_descr)
		free(LoginParams.status_descr);

	if (Sess)
	{
		SocketNotifiers->setSession(Sess);
		SocketNotifiers->start();
	}
	else
	{
		whileConnecting = false;
		disconnectedSlot();
		emit error(Disconnected);
	}

	kdebugf2();
}

void GaduProtocol::logout()
{
	kdebugf();

	whileConnecting = false;
	disconnectedSlot();

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

int GaduProtocol::sendMessage(UserListElements users, const char* msg)
{
	kdebugf();
	int seq = 0;
	unsigned int uinsCount = 0;
	CONST_FOREACH(user, users)
		if ((*user).usesProtocol("Gadu"))
			++uinsCount;
	if (uinsCount > 1)
	{
		UinType* uins = new UinType[uinsCount];
		unsigned int i = 0;
		CONST_FOREACH(user, users)
			if ((*user).usesProtocol("Gadu"))
				uins[i++] = (*user).ID("Gadu").toUInt();
		seq = gg_send_message_confer(Sess, GG_CLASS_CHAT,
			uinsCount, uins, (unsigned char *)msg);
		delete[] uins;
	}
	else
	{
		CONST_FOREACH(user, users)
			if ((*user).usesProtocol("Gadu"))
			{
				seq = gg_send_message(Sess, GG_CLASS_CHAT,
					(*user).ID("Gadu").toUInt(), (unsigned char*) msg);
				break;
			}
	}

	SocketNotifiers->checkWrite();

	kdebugf2();
	return seq;
}

int GaduProtocol::sendMessageRichText(UserListElements users, const char* msg, unsigned char* myLastFormats, unsigned int myLastFormatsLength)
{
	kdebugf();
	int seq = 0;
	unsigned int uinsCount = 0;
	CONST_FOREACH(user, users)
		if ((*user).usesProtocol("Gadu"))
			++uinsCount;
	if (uinsCount > 1)
	{
		UinType* uins = new UinType[uinsCount];
		unsigned int i = 0;
		CONST_FOREACH(user, users)
			if ((*user).usesProtocol("Gadu"))
				uins[i++] = (*user).ID("Gadu").toUInt();
		seq = gg_send_message_confer_richtext(Sess, GG_CLASS_CHAT,
				uinsCount, uins, (unsigned char*)msg,
				myLastFormats, myLastFormatsLength);
		delete[] uins;
	}
	else
		CONST_FOREACH(user, users)
			if ((*user).usesProtocol("Gadu"))
			{
				seq = gg_send_message_richtext(Sess, GG_CLASS_CHAT,
					(*user).ID("Gadu").toUInt(), (unsigned char*)msg,
					myLastFormats, myLastFormatsLength);
				break;
			}

	SocketNotifiers->checkWrite();

	kdebugf2();
	return seq;
}

void GaduProtocol::ackReceived(int seq, uin_t uin, int status)
{
	kdebugf();
	switch (status)
	{
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %d, seq: %d)\n", uin, seq);
			emit messageBlocked(seq, uin);
			emit messageRejected(seq, uin);
			break;
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %d, seq: %d)\n", uin, seq);
			emit messageDelivered(seq, uin);
			emit messageAccepted(seq, uin);
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, seq);
			emit messageQueued(seq, uin);
			emit messageAccepted(seq, uin);
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %d, seq: %d)\n", uin, seq);
			emit messageBoxFull(seq, uin);
			emit messageRejected(seq, uin);
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %d, seq: %d)\n", uin, seq);
			emit messageNotDelivered(seq, uin);
			emit messageRejected(seq, uin);
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, seq, status);
			break;
	}
	kdebugf2();
}

void GaduProtocol::sendUserListLater()
{
//	kdebugf();
	SendUserListTimer->start(0, true);
//	kdebugf2();
}

void GaduProtocol::sendUserList()
{
	kdebugf();
	UinType *uins;
	char *types;

	unsigned int j = 0;
	CONST_FOREACH(user, *userlist)
		if ((*user).usesProtocol("Gadu") && !(*user).isAnonymous())
			++j;

	if (!j)
	{
		gg_notify_ex(Sess, NULL, NULL, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = new UinType[j];
	types = new char[j];

	j = 0;
	CONST_FOREACH(user, *userlist)
		if ((*user).usesProtocol("Gadu") && !(*user).isAnonymous())
		{
			uins[j] = (*user).ID("Gadu").toUInt();
			if ((*user).protocolData("Gadu", "OfflineTo").toBool())
				types[j] = GG_USER_OFFLINE;
			else
				if ((*user).protocolData("Gadu", "Blocking").toBool())
					types[j] = GG_USER_BLOCKED;
				else
					types[j] = GG_USER_NORMAL;
			++j;
		}

	gg_notify_ex(Sess, uins, types, j);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;
	kdebugf2();
}

bool GaduProtocol::sendImageRequest(UserListElement user,int size,uint32_t crc32)
{
	kdebugf();
	int res = 1;
	if (user.usesProtocol("Gadu"))
		res = gg_image_request(Sess, user.ID("Gadu").toUInt(), size, crc32);
	kdebugf2();
	return (res == 0);
}

bool GaduProtocol::sendImage(UserListElement user, const QString& file_name, uint32_t size, const char* data)
{
	kdebugf();
	int res = 1;
	if (user.usesProtocol("Gadu"))
		res = gg_image_reply(Sess, user.ID("Gadu").toUInt(), file_name.local8Bit().data(), data, size);
	kdebugf2();
	return (res == 0);
}

/* wyszukiwanie w katalogu publicznym */

void GaduProtocol::searchInPubdir(SearchRecord& searchRecord)
{
	kdebugf();
	searchRecord.FromUin = 0;
	searchNextInPubdir(searchRecord);
	kdebugf2();
}

void GaduProtocol::searchNextInPubdir(SearchRecord& searchRecord)
{
	kdebugf();
	QString bufYear;
	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (!searchRecord.Uin.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(searchRecord.Uin).data());
	if (!searchRecord.FirstName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(searchRecord.FirstName).data());
	if (!searchRecord.LastName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(searchRecord.LastName).data());
	if (!searchRecord.NickName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(searchRecord.NickName).data());
	if (!searchRecord.City.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(searchRecord.City).data());
	if (!searchRecord.BirthYearFrom.isEmpty())
	{
		QString bufYear = searchRecord.BirthYearFrom + " " + searchRecord.BirthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufYear).data());
	}

	switch (searchRecord.Gender)
	{
		case 1:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case 2:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
	}

	if (searchRecord.Active)
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);

	gg_pubdir50_add(req, GG_PUBDIR50_START, QString::number(searchRecord.FromUin).local8Bit());

	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::newResults(gg_pubdir50_t res)
{
	kdebugf();
	int count, fromUin;
	SearchResult searchResult;
	SearchResults searchResults;

	count = gg_pubdir50_count(res);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; ++i)
	{
		searchResult.setData(
			gg_pubdir50_get(res, i, GG_PUBDIR50_UIN),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_LASTNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR),
			gg_pubdir50_get(res, i, GG_PUBDIR50_CITY),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYCITY),
			gg_pubdir50_get(res, i, GG_PUBDIR50_GENDER),
			gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS)
		);
		searchResults.append(searchResult);
	}
	fromUin = gg_pubdir50_next(res);

	emit newSearchResults(searchResults, res->seq, fromUin);
	kdebugf2();
}

/* informacje osobiste */

void GaduProtocol::getPersonalInfo(SearchRecord& searchRecord)
{
	kdebugf();

	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_READ);
	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData)
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
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(newData.Gender).latin1());
	if (!newData.FamilyName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(newData.FamilyName).data()));
	if (!newData.FamilyCity.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)(unicode2cp(newData.FamilyCity).data()));

	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

// -----------------------------
//      Zarz±dzanie kontem
// -----------------------------

void GaduProtocol::getToken()
{
	TokenSocketNotifiers *sn = new TokenSocketNotifiers(this, "token_socket_notifiers");
	connect(sn, SIGNAL(tokenError()), this, SLOT(tokenError()));
	connect(sn, SIGNAL(gotToken(QString, QPixmap)), this, SLOT(gotToken(QString, QPixmap)));
	sn->start();
}

void GaduProtocol::registerAccount(const QString &mail, const QString &password)
{
	kdebugf();

	Mode = Register;
	DataEmail = mail;
	DataPassword = password;
	getToken();

	kdebugf2();
}

void GaduProtocol::unregisterAccount(UinType uin, const QString &password)
{
	kdebugf();

	Mode = Unregister;
	DataUin = uin;
	DataPassword = password;
	getToken();

	kdebugf2();
}

void GaduProtocol::remindPassword(UinType uin, const QString& mail)
{
	kdebugf();

	Mode = RemindPassword;
	DataUin = uin;
	DataEmail = mail;
	getToken();

	kdebugf2();
}

void GaduProtocol::changePassword(UinType uin, const QString &mail, const QString &password,
	const QString &newPassword)
{
	kdebugf();

	Mode = ChangePassword;
	DataUin = uin;
	DataEmail = mail;
	DataPassword = password;
	DataNewPassword = newPassword;
	getToken();

	kdebugf2();
}

void GaduProtocol::doRegisterAccount()
{
	kdebugf();
	struct gg_http *h = gg_register3(unicode2cp(DataEmail).data(), unicode2cp(DataPassword).data(),
		unicode2cp(TokenId).data(), unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h, this, "pubdir_socket_notifiers");
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(registerDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit registered(false, 0);
	kdebugf2();
}

void GaduProtocol::registerDone(bool ok, struct gg_http *h)
{
	kdebugf();
	emit registered(ok, ok ? (((struct gg_pubdir *)h->data)->uin) : 0);
	kdebugf2();
}

void GaduProtocol::doUnregisterAccount()
{
	kdebugf();
	struct gg_http* h = gg_unregister3(DataUin, unicode2cp(DataPassword).data(), unicode2cp(TokenId).data(),
		unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h, this, "pubdir_socket_notifiers");
		connect(sn, SIGNAL(done(bool, struct gg_http *)),
			this, SLOT(unregisterDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit unregistered(false);
	kdebugf2();
}

void GaduProtocol::unregisterDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit unregistered(ok);
	kdebugf2();
}

void GaduProtocol::doRemindPassword()
{
	kdebugf();

	struct gg_http *h = gg_remind_passwd3(DataUin, unicode2cp(DataEmail).data(), unicode2cp(TokenId).data(),
		unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h, this, "pubdir_socket_notifiers");
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(remindDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit reminded(false);
	kdebugf2();
}

void GaduProtocol::remindDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit reminded(ok);
	kdebugf2();
}

void GaduProtocol::doChangePassword()
{
	kdebugf();

	struct gg_http *h = gg_change_passwd4(DataUin, unicode2cp(DataEmail).data(),
		unicode2cp(DataPassword).data(), unicode2cp(DataNewPassword).data(),
		unicode2cp(TokenId).data(), unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h, this, "pubdir_socket_notifiers");
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this,
			SLOT(changePasswordDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit passwordChanged(false);
	kdebugf2();
}

void GaduProtocol::changePasswordDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit passwordChanged(ok);
	kdebugf2();
}

/* tokeny */

void GaduProtocol::tokenError()
{
	kdebugf();
	switch (Mode)
	{
		case Register:
			emit registered(false, 0);
			break;
		case Unregister:
			emit unregistered(false);
			break;
		case RemindPassword:
			emit reminded(false);
			break;
		case ChangePassword:
			emit passwordChanged(false);
			break;
	}
	kdebugf2();
}

void GaduProtocol::gotToken(QString tokenId, QPixmap tokenImage)
{
	kdebugf();

	QString tokenValue;
	emit needTokenValue(tokenImage, tokenValue);

	TokenId = tokenId;
	TokenValue = tokenValue;

	switch (Mode)
	{
		case Register:
			doRegisterAccount();
			break;
		case Unregister:
			doUnregisterAccount();
			break;
		case RemindPassword:
			doRemindPassword();
			break;
		case ChangePassword:
			doChangePassword();
			break;
	}

	kdebugf2();
}

/* lista u¿ytkowników */

QString GaduProtocol::userListToString(const UserList& userList) const
{
	kdebugf();
	NotifyType type;
	QString file;
	QString contacts;

	CONST_FOREACH(i, userList)
		if (!(*i).isAnonymous() && ((*i).usesProtocol("Gadu") || !(*i).mobile().isEmpty()))
		{
			contacts += (*i).firstName();					contacts += ";";
			contacts += (*i).lastName();					contacts += ";";
			contacts += (*i).nickName();					contacts += ";";
			contacts += (*i).altNick();						contacts += ";";
			contacts += (*i).mobile();						contacts += ";";
			contacts += (*i).data("Groups").toStringList().join(";");	contacts += ";";
			if ((*i).usesProtocol("Gadu"))
				contacts += (*i).ID("Gadu");				contacts += ";";
			contacts += (*i).email();						contacts += ";";
			file = (*i).aliveSound(type);
			contacts += QString::number(type);				contacts += ";";
			contacts += file;								contacts += ";";
			file = (*i).messageSound(type);
			contacts += QString::number(type);				contacts += ";";
			contacts += file;								contacts += ";";
			if ((*i).usesProtocol("Gadu"))
				contacts += QString::number((*i).protocolData("Gadu", "OfflineTo").toBool());
			contacts += ";";
			contacts += (*i).homePhone();					//contacts += ";";
			contacts += "\r\n";
		}

	contacts.remove("(null)");

//	kdebugm(KDEBUG_DUMP, "%s\n", contacts.local8Bit().data());
	kdebugf2();
	return contacts;
}

QValueList<UserListElement> GaduProtocol::stringToUserList(const QString &string) const
{
	QString s = string;
	QTextStream stream(&s, IO_ReadOnly);
	return streamToUserList(stream);
}

QValueList<UserListElement> GaduProtocol::streamToUserList(QTextStream& stream) const
{
	kdebugf();

	QStringList sections, groupNames;
	QString line;
	QValueList<UserListElement> ret;
	unsigned int i, secCount;
	bool ok;

	stream.setCodec(codec_latin2);

	while (!stream.eof())
	{
		UserListElement e;
		line = stream.readLine();
//		kdebugm(KDEBUG_DUMP, ">>%s\n", line.local8Bit().data());
		sections = QStringList::split(";", line, true);
		secCount = sections.count();

		if (secCount < 7)
			continue;

		e.setFirstName(sections[0]);
		e.setLastName(sections[1]);
		e.setNickName(sections[2]);
		e.setAltNick(sections[3]);
		e.setMobile(sections[4]);

		groupNames.clear();
		if (!sections[5].isEmpty())
			groupNames.append(sections[5]);

		i = 6;
		ok = false;
		while (!ok && i < secCount)
		{
//			kdebugm(KDEBUG_DUMP, "checking: '%s'\n", sections[i].local8Bit().data());
			sections[i].toULong(&ok);
			ok = ok || sections[i].isEmpty();
			if (!ok)
			{
//				kdebugm(KDEBUG_DUMP, "adding: '%s'\n", sections[i].local8Bit().data());
				groupNames.append(sections[i]);
			}
			++i;
		}
		e.setData("Groups", groupNames);
		--i;

		if (i < secCount)
		{
			UinType uin = sections[i++].toULong(&ok);
			if (!ok)
				uin = 0;
			if (uin)
				e.addProtocol("Gadu", QString::number(uin));
		}

		if (i < secCount)
			e.setEmail(sections[i++]);
		if (i+1 < secCount)
		{
			e.setAliveSound((NotifyType)sections[i].toInt(), sections[i+1]);
			i+=2;
		}
		if (i+1 < secCount)
		{
			e.setMessageSound((NotifyType)sections[i].toInt(), sections[i+1]);
			i+=2;
		}
		if (i < secCount)
		{
			if (e.usesProtocol("Gadu"))
				e.setProtocolData("Gadu", "OfflineTo", bool(sections[i].toInt()));
			i++;
		}
		if (i < secCount)
			e.setHomePhone(sections[i++]);

		ret.append(e);
	}
	kdebugf2();
	return ret;
}

void GaduProtocol::connectAfterOneSecond()
{
	kdebugf();
	QTimer::singleShot(1000, this, SLOT(login()));
	kdebugf2();
}

bool GaduProtocol::doExportUserList(const UserList& userList)
{
	kdebugf();

	QString contacts = userListToString(userList);
	char *dup = strdup(unicode2cp(contacts));

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", dup);
//	free(dup);

	UserListClear = false;

//	dup = strdup(unicode2std(contacts));
	bool success=(gg_userlist_request(Sess, GG_USERLIST_PUT, dup)!=-1);
	if (!success)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "failed\n");
		emit userListExported(false);
	}
	free(dup);
	kdebugf2();
	return success;
}

bool GaduProtocol::doClearUserList()
{
	kdebugf();

	UserListClear = true;

	char *dup = "";
	bool success=(gg_userlist_request(Sess, GG_USERLIST_PUT, dup) != -1);
	if (!success)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "failed\n");
		emit userListCleared(false);
	}
	kdebugf2();
	return success;
}

bool GaduProtocol::doImportUserList()
{
	kdebugf();

	ImportReply.truncate(0);

	bool success=(gg_userlist_request(Sess, GG_USERLIST_GET, NULL) != -1);
	if (!success)
		emit userListImported(false, QValueList<UserListElement>());
	kdebugf2();
	return success;
}

void GaduProtocol::userListReceived(const struct gg_event *e)
{
	kdebugf();

	GaduStatus oldStatus;
	int nr = 0;
	//kdebugm(KDEBUG_WARNING, "%s\n", userListToString(*userlist).local8Bit().data());
	//return;

	int cnt = 0;
	while (e->event.notify60[nr].uin) // zliczamy najpierw ile zmian statusów zostanie wyemitowanych
	{
		if (!userlist->byID("Gadu", QString::number(e->event.notify60[nr].uin)).isAnonymous())
			++cnt;
		++nr;
	}
	nr = 0;
	//a teraz bêdziemy przetwarzaæ

	while (e->event.notify60[nr].uin)
	{
		UserListElement user = userlist->byID("Gadu", QString::number(e->event.notify60[nr].uin));

		if (user.isAnonymous())
		{
			kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n",
				e->event.notify60[nr].uin);
			gg_remove_notify(Sess, e->event.notify60[nr].uin);
			++nr;
			continue;
		}

		user.setProtocolData("Gadu", "DNSName", QString::null, nr + 1 == cnt);
		user.setProtocolData("Gadu", "IP", ntohl(e->event.notify60[nr].remote_ip), nr + 1 == cnt);
		user.setProtocolData("Gadu", "Port", e->event.notify60[nr].remote_port, nr + 1 == cnt);
		user.refreshDNSName("Gadu");

		user.setProtocolData("Gadu", "Version", e->event.notify60[nr].version, true, nr + 1 == cnt);
		user.setProtocolData("Gadu", "MaxImageSize", e->event.notify60[nr].image_size, true, nr + 1 == cnt);

		oldStatus = user.status("Gadu");

		GaduStatus status;
		if (e->event.notify60[nr].descr)
		{
			status.fromStatusNumber(e->event.notify60[nr].status,
				cp2unicode((unsigned char *)e->event.notify60[nr].descr));
			QString desc = status.description();
			desc.replace("\r\n", "\n");
			desc.replace("\r", "\n");
			status.setDescription(desc);
		}
		else
			status.fromStatusNumber(e->event.notify60[nr].status, QString::null);
		user.setStatus("Gadu", status, true, nr + 1 == cnt);

#ifdef DEBUG_ENABLED
#define PRINT_STAT(str) kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, str, e->event.notify60[nr].uin);
		switch (e->event.notify60[nr].status)
		{
			case GG_STATUS_AVAIL:			PRINT_STAT("User %d went online\n");					break;
			case GG_STATUS_BUSY:			PRINT_STAT("User %d went busy\n");						break;
			case GG_STATUS_NOT_AVAIL:		PRINT_STAT("User %d went offline\n");					break;
			case GG_STATUS_BLOCKED:			PRINT_STAT("User %d has blocked us\n");					break;
			case GG_STATUS_BUSY_DESCR:		PRINT_STAT("User %d went busy with description\n");		break;
			case GG_STATUS_NOT_AVAIL_DESCR:	PRINT_STAT("User %d went offline with description\n");	break;
			case GG_STATUS_AVAIL_DESCR:		PRINT_STAT("User %d went online with description\n");	break;
			case GG_STATUS_INVISIBLE_DESCR:	PRINT_STAT("User %d went invisible with description\n");break;
			default:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unknown status for user %d: %d\n", e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
		}
#endif

		++nr;
	}

	kdebugf2();
}

void GaduProtocol::userListReplyReceived(char type, char *reply)
{
	kdebugf();

	if (type == GG_USERLIST_PUT_REPLY)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Done\n");

		if (UserListClear)
			emit userListCleared(true);
		else
			emit userListExported(true);

	}
	else if ((type == GG_USERLIST_GET_REPLY) || (type == GG_USERLIST_GET_MORE_REPLY))
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "get\n");

		if (!reply)
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");

			emit userListImported(false, QValueList<UserListElement>());
			return;
		}

		if (reply[0] != 0)
			ImportReply += cp2unicode((unsigned char *)reply);

		if (type == GG_USERLIST_GET_MORE_REPLY)
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "next portion\n");
			return;
		}

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", unicode2latin(ImportReply).data());

		emit userListImported(true, stringToUserList(ImportReply));
	}

	kdebugf2();
}

void GaduProtocol::userStatusChanged(const struct gg_event *e)
{
	kdebugf();

	GaduStatus oldStatus, status;
	uint32_t uin;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint8_t version;
	uint8_t image_size;

	if (e->type == GG_EVENT_STATUS60)
	{
		uin = e->event.status60.uin;
		status.fromStatusNumber(e->event.status60.status,
			cp2unicode((unsigned char *)e->event.status60.descr));
		remote_ip = e->event.status60.remote_ip;
		remote_port = e->event.status60.remote_port;
		version = e->event.status60.version;
		image_size = e->event.status60.image_size;
	}
	else
	{
		uin = e->event.status.uin;
		status.fromStatusNumber(e->event.status.status,
			cp2unicode((unsigned char *)e->event.status.descr));
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	QString desc = status.description();
	desc.replace("\r\n", "\n");
	desc.replace("\r", "\n");
	status.setDescription(desc);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went %d (%s)\n", uin,
		status.toStatusNumber(), status.name().local8Bit().data());

	if (!userlist->contains("Gadu", QString::number(uin), FalseForAnonymous))
	{
		// ignore!
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(Sess, uin);
		emit userStatusChangeIgnored(uin);
		return;
	}

	UserListElement user = userlist->byID("Gadu", QString::number(uin));

	if (status.isOffline())
	{
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}
	user.setAddressAndPort("Gadu", ntohl(remote_ip), remote_port);
	user.setProtocolData("Gadu", "Version", version);
	user.setProtocolData("Gadu", "MaxImageSize", image_size);

	user.refreshDNSName("Gadu");

	oldStatus = user.status("Gadu");
	user.setStatus("Gadu", status, false, false);

	kdebugf2();
}

void GaduProtocol::freeEvent(struct gg_event* e)
{
	gg_free_event(e);
}

void GaduProtocol::dccRequest(UinType uin)
{
	gg_dcc_request(Sess, uin);
}

void GaduProtocol::dccFree(struct gg_dcc* d)
{
	gg_dcc_free(d);
}

struct gg_event* GaduProtocol::dccWatchFd(struct gg_dcc* d)
{
	return gg_dcc_watch_fd(d);
}

void GaduProtocol::dccSetType(struct gg_dcc* d, int type)
{
	gg_dcc_set_type(d, type);
}

int GaduProtocol::dccFillFileInfo(struct gg_dcc* d, const QString& filename)
{
	return gg_dcc_fill_file_info2(d, unicode2cp(filename), filename.local8Bit().data());
}

void GaduProtocol::dccSocketCreate(UinType uin, uint16_t port, struct gg_dcc **out)
{
	*out = gg_dcc_socket_create(uin, port);
}

void GaduProtocol::dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out)
{
	*out = gg_dcc_send_file(ip, port, my_uin, peer_uin);
}

void GaduProtocol::dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out)
{
	*out = gg_dcc_get_file(ip, port, my_uin, peer_uin);
}

void GaduProtocol::dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out)
{
	*out = gg_dcc_voice_chat(ip, port, my_uin, peer_uin);
}

int GaduProtocol::dccVoiceSend(struct gg_dcc* d, char* buf, int length)
{
	return gg_dcc_voice_send(d, buf, length);
}

void GaduProtocol::setDccIpAndPort(unsigned long dcc_ip, int dcc_port)
{
	gg_dcc_ip = dcc_ip;
	gg_dcc_port = dcc_port;
}

void GaduProtocol::onCreateTabNetwork()
{
	kdebugf();

	QVGroupBox *g_proxy = ConfigDialog::getVGroupBox("Network", "Proxy server");
	QCheckBox *b_useproxy= ConfigDialog::getCheckBox("Network", "Use proxy server");
	ConfigDialog::getLineEdit("Network"," Password: ")->setEchoMode(QLineEdit::Password);

#ifdef HAVE_OPENSSL
	QCheckBox *b_tls= ConfigDialog::getCheckBox("Network", "Use TLSv1");
#endif
	QWidget *serverbox = ConfigDialog::getEntireWidget("Network", "IP addresses:","server");
	QCheckBox* b_defaultserver= ConfigDialog::getCheckBox("Network", "Use default servers");

	g_proxy->setEnabled(b_useproxy->isChecked());

	serverbox->setEnabled(!b_defaultserver->isChecked());

	connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));

	kdebugf2();
}

void GaduProtocol::onApplyTabNetwork()
{
	kdebugf();

	QLineEdit *e_servers=ConfigDialog::getLineEdit("Network", "IP addresses:", "server");

	QStringList tmpservers,server;
	QValueList<QHostAddress> servers;
	QHostAddress ip;
	bool ipok;

	tmpservers = QStringList::split(";", e_servers->text());
	CONST_FOREACH(tmpserver, tmpservers)
	{
		ipok = ip.setAddress(*tmpserver);
		if (!ipok)
			continue;
		servers.append(ip);
		server.append(ip.toString());
	}
	config_file.writeEntry("Network","Server",server.join(";"));
	ConfigServers = servers;
	ServerNr = 0;
	changeID(QString::number(config_file.readNumEntry("General", "UIN")));

	kdebugf2();
}

void GaduProtocol::ifDefServerEnabled(bool value)
{
	kdebugf();
	ConfigDialog::getEntireWidget("Network", "IP addresses:","server")->setEnabled(!value);
	kdebugf2();
}

void GaduProtocol::useTlsEnabled(bool value)
{
#ifdef HAVE_OPENSSL
	kdebugf();
	ConfigDialog::getEntireWidget("Network", "Port to connect to servers")->setEnabled(!value);
	kdebugf2();
#endif
}

GaduStatus::GaduStatus()
{
}

GaduStatus::~GaduStatus()
{
}

void GaduStatus::operator = (const UserStatus &copyMe)
{
	setStatus(copyMe);
}

QPixmap GaduStatus::pixmap(eUserStatus stat, bool hasDescription, bool mobile) const
{
//	kdebugf();

	QString add = (hasDescription ? "WithDescription" : QString::null);
	add.append(mobile ? (!hasDescription) ? "WithMobile" : "Mobile" : "");

	switch (stat)
	{
		case Online:
			return icons_manager->loadIcon(QString("Online").append(add));
		case Busy:
			return icons_manager->loadIcon(QString("Busy").append(add));
		case Invisible:
			return icons_manager->loadIcon(QString("Invisible").append(add));
		case Blocking:
			return icons_manager->loadIcon(QString("Blocking"));
		default:
			return icons_manager->loadIcon(QString("Offline").append(add));
	}
}

int GaduStatus::toStatusNumber() const
{
	return toStatusNumber(Stat, !Description.isEmpty());
}

int GaduStatus::toStatusNumber(eUserStatus status, bool has_desc)
{
	int sn = 0;

	switch (status)
	{
		case Online:
			sn = has_desc ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;
			break;

		case Busy:
			sn = has_desc ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;
			break;

		case Invisible:
			sn = has_desc ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;
			break;

		case Blocking:
			sn = GG_STATUS_BLOCKED;
			break;

		case Offline:
		default:
			sn = has_desc ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
			break;
	}

	return sn;
}

void GaduStatus::fromStatusNumber(int statusNumber, const QString &description)
{
	Description.truncate(0);

	switch (statusNumber)
	{
		case GG_STATUS_AVAIL_DESCR:
			Description = description;
		case GG_STATUS_AVAIL:
			Stat = Online;
			break;

		case GG_STATUS_BUSY_DESCR:
			Description = description;
		case GG_STATUS_BUSY:
			Stat = Busy;
			break;

		case GG_STATUS_INVISIBLE_DESCR:
			Description = description;
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			Stat = Invisible;
			break;

		case GG_STATUS_BLOCKED:
			Stat = Blocking;
			break;

		case GG_STATUS_NOT_AVAIL_DESCR:
			Description = description;
		case GG_STATUS_NOT_AVAIL:
		default:
			Stat = Offline;
			break;
	}
}

UserStatus *GaduStatus::copy() const
{
	return new GaduStatus(*this);
}

QString GaduStatus::protocolName() const
{
	static const QString protoName("Gadu");
	return protoName;
}

GaduProtocol* gadu;
