/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu.h"
#include "userlist.h"
#include "debug.h"
#include "kadu.h"
#include "chat.h"
#include "ignore.h"
#include "config_dialog.h"
#include "message_box.h"

#include <netinet/in.h>

#include <qregexp.h>
#include <qsocketnotifier.h>
#include <qtextcodec.h>

#include <stdlib.h>

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

QValueList<QHostAddress> gg_servers;
const char *gg_servers_ip[9] = {"217.17.41.82", "217.17.41.83", "217.17.41.84", "217.17.41.85",
	"217.17.41.86", "217.17.41.87", "217.17.41.88", "217.17.41.92", "217.17.41.93"};

QHostAddress config_extip;

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
	start(5000, TRUE);
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
	for (UinsList::const_iterator i = begin(); i != end(); ++i)
		if(!uins.contains(*i))
			return false;
	return true;
}

UinsList::UinsList()
{
}

UinsList::UinsList (const QString &uins)
{
	QStringList list = QStringList::split (",", uins);
	for (QStringList::const_iterator it = list.begin (); it != list.end (); it++)
		append ((*it).toUInt ());
}

UinsList::UinsList (const QStringList &list)
{
	for (QStringList::const_iterator it = list.begin (); it != list.end (); it++)
		append ((*it).toUInt ());
}

void UinsList::sort()
{
	qHeapSort(*this);
}

QStringList UinsList::toStringList() const
{
	QStringList list;
	for (UinsList::const_iterator i = begin(); i != end(); ++i)
		list.append(QString::number(*i));
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
		Stat.fromStatusNumber(atoi(status) & 127, "");
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
	Uin = "";
	FirstName = "";
	LastName = "";
	NickName = "";
	City = "";
	BirthYearFrom = "";
	BirthYearTo = "";
	Gender = 0;
	Active = false;
	kdebugf2();
}
/* SocketNotifiers */

SocketNotifiers::SocketNotifiers(int fd, QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	Fd = fd;
	Snr = Snw = NULL;
	kdebugf2();
}

SocketNotifiers::~SocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::start()
{
	kdebugf();
	createSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::stop()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	Snr = new QSocketNotifier(Fd, QSocketNotifier::Read);
	connect(Snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	Snw = new QSocketNotifier(Fd, QSocketNotifier::Write);
	connect(Snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

	kdebugf2();
}

void SocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (Snr)
	{
		Snr->setEnabled(false);
		Snr->deleteLater();
		Snr = NULL;
	}

	if (Snw)
	{
		Snw->setEnabled(false);
		Snw->deleteLater();
		Snw = NULL;
	}

	kdebugf2();
}

void SocketNotifiers::recreateSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();
	createSocketNotifiers();

	kdebugf2();
}

/* PubdirSocketNotifiers */

PubdirSocketNotifiers::PubdirSocketNotifiers(struct gg_http *h, QObject *parent, const char *name)
	: SocketNotifiers(h->fd, parent, name)
{
	kdebugf();
	H = h;
	kdebugf2();
}

PubdirSocketNotifiers::~PubdirSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void PubdirSocketNotifiers::dataReceived()
{
	kdebugf();

	if (H->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void PubdirSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void PubdirSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_pubdir_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit done(false, H);
		gg_pubdir_free(H);
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{
		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers\n");
			recreateSocketNotifiers();

			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");
			deleteSocketNotifiers();
			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();

			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success!\n");
				emit done(true, H);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");
				emit done(false, H);
			}
			gg_pubdir_free(H);
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
	}
	kdebugf2();
}

/* TokenSocketNotifier */

TokenSocketNotifiers::TokenSocketNotifiers(QObject *parent, const char *name)
	: SocketNotifiers(0, parent, name)
{
	kdebugf();
	kdebugf2();
}

TokenSocketNotifiers::~TokenSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void TokenSocketNotifiers::start()
{
	kdebugf();

	if (!(H = gg_token(1)))
	{
		emit tokenError();
		return;
	}

	Fd = H->fd;
	createSocketNotifiers();
	kdebugf2();
}

void TokenSocketNotifiers::dataReceived()
{
	kdebugf();

	if (H->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void TokenSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void TokenSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_token_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit tokenError();
		gg_token_free(H);
		H = NULL;
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{

		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
			break;

		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			emit tokenError();
			gg_token_free(H);
			H = NULL;
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();
			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success\n");

				struct gg_token *t = (struct gg_token *)H->data;
				QString tokenId = cp2unicode((unsigned char *)t->tokenid);

				//nie optymalizowac!!!
				QByteArray buf(H->body_size);
				for (unsigned int i = 0; i < H->body_size; ++i)
					buf[i] = H->body[i];

				QPixmap tokenImage(buf);

				emit gotToken(tokenId, tokenImage);
			}

			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
				emit tokenError();
			}

			gg_token_free(H);
			H = NULL;
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
	}

	kdebugf2();
}

/* GaduSocketNotifiers */

GaduSocketNotifiers::GaduSocketNotifiers(QObject *parent, const char *name) : SocketNotifiers(0, parent, name)
{
	kdebugf();
	Sess = 0;
	kdebugf2();
}

GaduSocketNotifiers::~GaduSocketNotifiers()
{
}

void GaduSocketNotifiers::setSession(gg_session *sess)
{
	Sess = sess;
	Fd = Sess->fd;
}

void GaduSocketNotifiers::checkWrite()
{
	kdebugf();
	//gdzie¶ tu siê sypie :|
	if (Sess==NULL)
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
	if (Sess->check & GG_CHECK_WRITE)
	{
		if (Snw==NULL)
			kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");
		Snw->setEnabled(true);
	}
	kdebugf2();
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	if (Sess->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	if (Sess==NULL)
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
	if (Snw==NULL)
		kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");

	Snw->setEnabled(false);
	if (Sess->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void GaduSocketNotifiers::socketEvent()
{
	kdebugf();
	static int calls = 0;

	++calls;
	if (calls > 1)
		kdebugm(KDEBUG_WARNING, "************* GaduSocketNotifiers::socketEvent(): Recursive socketEvent calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(Sess)))
	{
		emit error(ConnectionUnknow);
//		gg_free_event(e);//nulla nie zwalniamy, bo i po co?
		--calls;
		return;
	}

	if (Sess->state == GG_STATE_CONNECTING_HUB || Sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");

		recreateSocketNotifiers();
	}

	switch (Sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Waiting for hash key\n");
			ConnectionTimeoutTimer::off();
			break;
		case GG_STATE_READING_REPLY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Sending key\n");
			ConnectionTimeoutTimer::off();
			break;
		case GG_STATE_IDLE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "idle!\n");
			break;
		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "state==error! error=%d\n", Sess->error);
			break;
		case GG_STATE_CONNECTED:
			break;
		default:
			break;
	}

	if (e->type == GG_EVENT_MSG)
	{
		UinsList uins;
		if (e->event.msg.msgclass == GG_CLASS_CTCP)
		{
			uins.append(e->event.msg.sender);
			if (config_file.readBoolEntry("Network", "AllowDCC") && !isIgnored(uins) && userlist.containsUin(e->event.msg.sender))
				emit dccConnectionReceived(userlist.byUin(e->event.msg.sender));
		}
		else
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);
			uins.append(e->event.msg.sender);
			if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT)
				for (int i = 0; i < e->event.msg.recipients_count; ++i)
					uins.append(e->event.msg.recipients[i]);
			QCString msg((char*)e->event.msg.message);
			QByteArray formats;
			formats.duplicate((const char*)e->event.msg.formats, e->event.msg.formats_length);
			emit messageReceived(e->event.msg.msgclass, uins, msg,
				e->event.msg.time, formats);
		}
	}

	else if (e->type == GG_EVENT_IMAGE_REQUEST)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image request received\n");
		emit imageRequestReceived(
			e->event.image_request.sender,
			e->event.image_request.size,
			e->event.image_request.crc32);
	}

	else if (e->type == GG_EVENT_IMAGE_REPLY)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image reply received\n");
		emit imageReceived(
			e->event.image_reply.sender,
			e->event.image_reply.size,
			e->event.image_reply.crc32,
			e->event.image_reply.filename,
			e->event.image_reply.image);
	}

	else if (e->type == GG_EVENT_STATUS60 || e->type == GG_EVENT_STATUS)
		emit userStatusChanged(e);

	else if (e->type == GG_EVENT_ACK)
	{
		emit ackReceived(e->event.ack.seq, e->event.ack.recipient, e->event.ack.status);
	}

	else if (e->type == GG_EVENT_NOTIFY60)
		emit userlistReceived(e);

	else if (e->type == GG_EVENT_PUBDIR50_SEARCH_REPLY
		|| e->type == GG_EVENT_PUBDIR50_READ || e->type == GG_EVENT_PUBDIR50_WRITE)
		emit pubdirReplyReceived(e->event.pubdir50);

	else if (e->type == GG_EVENT_USERLIST)
		emit userlistReplyReceived(e->event.userlist.type, e->event.userlist.reply);

	else if (e->type == GG_EVENT_CONN_SUCCESS)
		emit connected();

	else if (e->type == GG_EVENT_CONN_FAILED)
		connectionFailed(e->event.failure);

	else if (e->type == GG_EVENT_DISCONNECT)
		emit disconnected();

	else if (e->type == GG_EVENT_NONE)
		kdebugm (KDEBUG_NETWORK, "GG_EVENT_NONE\n");

	gg_free_event(e);
	--calls;
	kdebugf2();
}

void GaduSocketNotifiers::connectionFailed(int failure)
{
	kdebugf();
	GaduError err;

	switch (failure)
	{
		case GG_FAILURE_RESOLVING:	err = ConnectionServerNotFound; break;
		case GG_FAILURE_CONNECTING:	err = ConnectionCannotConnect; break;
		case GG_FAILURE_NEED_EMAIL:	err = ConnectionNeedEmail; break;
		case GG_FAILURE_INVALID:	err = ConnectionInvalidData; break;
		case GG_FAILURE_READING:	err = ConnectionCannotRead; break;
		case GG_FAILURE_WRITING:	err = ConnectionCannotWrite; break;
		case GG_FAILURE_PASSWORD:	err = ConnectionIncorrectPassword; break;
		case GG_FAILURE_TLS:		err = ConnectionTlsError; break;
		default:
			kdebugm(KDEBUG_ERROR, "ERROR: unhandled/unknown connection error! %d\n", failure);
			err=ConnectionUnknow;
	}

	emit error(err);
	kdebugf2();
}

/* GaduProtocol */

QValueList<QHostAddress> GaduProtocol::ConfigServers;

void GaduProtocol::initModule()
{
	kdebugf();
	gadu = new GaduProtocol(kadu, "gadu");

	QHostAddress ip;
	for (int i = 0; i < 9; ++i)
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
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Network"), "NetworkTab");

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Servers properties"));
	ConfigDialog::addGrid("Network", "Servers properties", "servergrid", 2);
	ConfigDialog::addCheckBox("Network", "servergrid",
		QT_TRANSLATE_NOOP("@default", "Use default servers"), "isDefServers", true);
#ifdef HAVE_OPENSSL
	ConfigDialog::addCheckBox("Network", "servergrid",
		QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
	ConfigDialog::addLineEdit("Network", "Servers properties",
		QT_TRANSLATE_NOOP("@default", "IP addresses:"), "Server","","","server");

	// opcja wed³ug niektórych przydatna
	ConfigDialog::addComboBox("Network", "Servers properties",
		QT_TRANSLATE_NOOP("@default", "Default port to connect to servers"));

	ConfigDialog::addCheckBox("Network", "Network",
		QT_TRANSLATE_NOOP("@default", "Use proxy server"), "UseProxy", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Proxy server"));
	ConfigDialog::addGrid("Network", "Proxy server", "proxygrid", 2);
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "IP address: "), "ProxyHost", "0.0.0.0","","proxyhost");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", " Port: "), "ProxyPort", "0");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", "Username: "), "ProxyUser");
	ConfigDialog::addLineEdit("Network", "proxygrid",
		QT_TRANSLATE_NOOP("@default", " Password: "), "ProxyPassword");

	ConfigDialog::registerSlotOnCreate(gadu, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(gadu, SLOT(onDestroyConfigDialog()));

	ConfigDialog::connectSlot("Network", "Use default servers", SIGNAL(toggled(bool)),
		gadu, SLOT(ifDefServerEnabled(bool)));
#ifdef HAVE_OPENSSL
	ConfigDialog::connectSlot("Network", "Use TLSv1", SIGNAL(toggled(bool)),
		gadu, SLOT(useTlsEnabled(bool)));
#endif

	defaultdescriptions = QStringList::split(QRegExp("<-->"), config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);

	QStringList servers;
	QHostAddress ip2;
	servers = QStringList::split(";", config_file.readEntry("Network","Server", ""));
	ConfigServers.clear();
	for (unsigned int i = 0; i < servers.count(); ++i)
		if (ip2.setAddress(servers[i]))
			ConfigServers.append(ip2);

	kdebugf2();
}

GaduProtocol::GaduProtocol(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

	Sess = NULL;
	CurrentStatus = new GaduStatus();
	NextStatus = new GaduStatus();
	SocketNotifiers = new GaduSocketNotifiers(NULL, "gadu_socket_notifiers");
	UserListSent = false;
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
	connect(SocketNotifiers, SIGNAL(messageReceived(int, UinsList, QCString &, time_t, QByteArray &)),
		this, SLOT(messageReceived(int, UinsList, QCString &, time_t, QByteArray &)));
	connect(SocketNotifiers, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)), this, SLOT(newResults(gg_pubdir50_t)));
	connect(SocketNotifiers, SIGNAL(systemMessageReceived(QString &, QDateTime &, int, void *)),
		this, SLOT(systemMessageReceived(QString &, QDateTime &, int, void *)));
	connect(SocketNotifiers, SIGNAL(userlistReceived(const struct gg_event *)),
		this, SLOT(userListReceived(const struct gg_event *)));
	connect(SocketNotifiers, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
	connect(SocketNotifiers, SIGNAL(userStatusChanged(const struct gg_event *)),
		this, SLOT(userStatusChanged(const struct gg_event *)));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnectedSlot();
	delete SocketNotifiers;

	kdebugf2();
}

UserStatus & GaduProtocol::status()
{
	return *NextStatus;
}

const UserStatus & GaduProtocol::currentStatus()
{
	return *CurrentStatus;
}

void GaduProtocol::iWantGoOnline(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
	{
		unsigned char *pdesc;
		pdesc = (unsigned char *)strdup(unicode2cp(desc).data());
		gg_change_status_descr(Sess, GG_STATUS_AVAIL_DESCR | friends, (const char *)pdesc);
		free(pdesc);
	}
	else
		gg_change_status(Sess, GG_STATUS_AVAIL | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoBusy(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
	{
		unsigned char *pdesc;
		pdesc = (unsigned char *)strdup(unicode2cp(desc).data());
		gg_change_status_descr(Sess, GG_STATUS_BUSY_DESCR | friends, (const char *)pdesc);
		free(pdesc);
	}
	else
		gg_change_status(Sess, GG_STATUS_BUSY | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoInvisible(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
	{
		unsigned char *pdesc;
		pdesc = (unsigned char *)strdup(unicode2cp(desc).data());
		gg_change_status_descr(Sess, GG_STATUS_INVISIBLE_DESCR | friends, (const char *)pdesc);
		free(pdesc);
	}
	else
		gg_change_status(Sess, GG_STATUS_INVISIBLE | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoOffline(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline())
		return;

	if (!desc.isEmpty())
	{
		unsigned char *pdesc;
		pdesc = (unsigned char *)strdup(unicode2cp(desc).data());
		gg_change_status_descr(Sess, GG_STATUS_NOT_AVAIL_DESCR, (const char *)pdesc);
		free(pdesc);
	}
	else
		gg_change_status(Sess, GG_STATUS_NOT_AVAIL);

	CurrentStatus->setStatus(*NextStatus);
	logout();

	kdebugf2();
}

void GaduProtocol::userDataChanged(const UserListElement* const oldData, const UserListElement* const newData)
{
	kdebugf();

	// nic
	if (CurrentStatus->isOffline())
		return;
	if (!oldData && !newData)
		return;
	if (oldData && newData)
		if (!oldData->uin() && !newData->uin())
			return;

	// nowy kontakt
	if (!oldData && newData)
	{
		if (newData->uin() && !newData->isAnonymous())
			gg_add_notify(Sess, newData->uin());
		return;
	}

	// usuwamy kontakt
	if (!newData && oldData)
	{
		if (oldData->uin())
			gg_remove_notify(Sess, oldData->uin());
		return;
	}

	if (newData->offlineTo() && !oldData->offlineTo())
	{
		if (oldData->uin()) gg_remove_notify_ex(Sess, oldData->uin(), GG_USER_NORMAL);
		if (newData->uin()) gg_add_notify_ex(Sess, newData->uin(), GG_USER_OFFLINE);
	}
	else if (!newData->offlineTo() && oldData->offlineTo())
	{
		if (oldData->uin()) gg_add_notify_ex(Sess, oldData->uin(), GG_USER_OFFLINE);
		if (newData->uin()) gg_add_notify_ex(Sess, newData->uin(), GG_USER_NORMAL);
	}
	else if (newData->blocking() && !oldData->blocking())
	{
		if (oldData->uin()) gg_remove_notify_ex(Sess, oldData->uin(), GG_USER_NORMAL);
		if (newData->uin()) gg_add_notify_ex(Sess, newData->uin(), GG_USER_BLOCKED);
	}
	else if (!newData->blocking() && oldData->blocking())
	{
		if (oldData->uin()) gg_remove_notify_ex(Sess, oldData->uin(), GG_USER_BLOCKED);
		if (newData->uin()) gg_add_notify_ex(Sess, newData->uin(), GG_USER_NORMAL);
	}
	else if (oldData->isAnonymous())
	{
		if (newData->uin()) gg_add_notify(Sess, newData->uin());
	}
	else if (oldData->uin() != newData->uin())
	{
		if (oldData->uin()) gg_remove_notify(Sess, oldData->uin());
		if (newData->uin()) gg_add_notify(Sess, newData->uin());
	}

	kdebugf2();
}

bool GaduProtocol::userListSent()
{
	return UserListSent;
}

QHostAddress* GaduProtocol::activeServer()
{
	return ActiveServer;
}

void GaduProtocol::connectedSlot()
{
	kdebugf();

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
	NextStatus->refresh();
	
	/*
		je¿eli robimy refresh(), to przy przechodzeniu z niedostêpnego z opisem
		na niewidoczny z opisem ta zmiana jest ujawniana naszym kontaktom!
		przy przechodzeniu z nidostêpnego na niewidoczny efekt nie wystêpuje
	
		je¿eli NIE zrobimy refresh(), to powy¿szy efekt nie wystêpuje, ale przy
		przechodzeniu z niedostêpnego z opisem na niewidoczny (bez opisu), nasz
		opis u innych pozostaje! (a¿ do czasu naszej zmiany statusu lub ich
		roz³±czenia i po³±czenia)
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

	UserListSent = false;

	for (UserList::Iterator i = userlist.begin(); i != userlist.end(); ++i)
		(*i).status().setOffline();

	CurrentStatus->setOffline("");
	emit disconnected();
	kdebugf2();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();
	if (Sess->state == GG_STATE_CONNECTING_HUB ||
		Sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
		errorSlot(ConnectionTimeout);
		//emit error(ConnectionTimeout);
		//logout();
		//login();
	}
	kdebugf2();
}

void GaduProtocol::errorSlot(GaduError err)
{
	kdebugf();

	disconnectedSlot();
	emit error(err);
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

void GaduProtocol::messageReceived(int msgclass, UinsList senders, QCString &msg, time_t time,
	QByteArray &formats)
{
/*
	najpierw sprawdzamy czy nie jest to wiadomosc systemowa (senders[0] rowne 0)
	potem sprawdzamy czy user jest na naszej liscie, jezeli nie to anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/
	if (senders[0] && userlist.byUinValue(senders[0]).isAnonymous() && config_file.readBoolEntry("Chat","IgnoreAnonymousUsers"))
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", senders[0]);
		return;
	}

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders))
		return;

	bool block = false;
	emit messageFiltering(senders,msg,formats,block);
	if(block)
		return;

	const char* msg_c = msg;
	QString mesg = cp2unicode((const unsigned char*)msg_c);
	QDateTime datetime;
	datetime.setTime_t(time);

	bool grab=false;
	emit chatMsgReceived0(senders, mesg, time, grab);
	if (grab)
		return;

	// wiadomosci systemowe maja senders[0] = 0
	// FIX ME!!!
	if (senders[0] == 0)
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

	mesg = formatGGMessage(mesg, formats.size(), formats.data(), senders[0]);
	if (mesg.isEmpty())
		return;

	if(!userlist.containsUin(senders[0]))
		userlist.addAnonymous(senders[0]);

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			senders[0], (const char *)mesg.local8Bit());

	emit chatMsgReceived1(senders, mesg, time, grab);
	if(!grab)
		emit chatMsgReceived2(senders, mesg, time);
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

	emit connecting();

	memset(&LoginParams, 0, sizeof(LoginParams));
	LoginParams.async = 1;

	// maksymalny rozmiar grafiki w kb
	LoginParams.image_size = config_file.readNumEntry("Chat", "MaxImageSize", 20);

	setupProxy();

	LoginParams.status = NextStatus->toStatusNumber();
	if (NextStatus->isFriendsOnly())
		LoginParams.status |= GG_STATUS_FRIENDS_MASK;
	if (NextStatus->hasDescription())
		LoginParams.status_descr = strdup((const char *)unicode2cp(NextStatus->description()).data());

	LoginParams.uin = (UinType)config_file.readNumEntry("General", "UIN");
	LoginParams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	// GG 6.0 build 147 ustawia indeks ostatnio odczytanej wiadomosci systemowej na 1389
	LoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	if (config_file.readBoolEntry("Network", "AllowDCC") && config_extip.ip4Addr() && config_file.readNumEntry("Network", "ExternalPort") > 1023)
	{
		LoginParams.external_addr = htonl(config_extip.ip4Addr());
		LoginParams.external_port = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		LoginParams.external_addr = 0;
		LoginParams.external_port = 0;
	}

	if (ConfigServers.count() && !config_file.readBoolEntry("Network", "isDefServers") && ConfigServers[ServerNr].ip4Addr())
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
		disconnectedSlot();
		emit error(Disconnected);
	}

	kdebugf2();
}

void GaduProtocol::logout()
{
	kdebugf();

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

		if (config_file.readEntry("Network", "ProxyUser").length())
		{
			gg_proxy_username = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyUser")).data());
			gg_proxy_password = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyPassword")).data());
		}
	}

	kdebugf2();
}

int GaduProtocol::sendMessage(const UinsList& uins, const char* msg)
{
	kdebugf();
	int seq;
	if(uins.count()>1)
	{
		UinType* users = new UinType[uins.count()];
		for (unsigned int i = 0; i < uins.count(); ++i)
			users[i] = uins[i];
		seq=gg_send_message_confer(Sess, GG_CLASS_CHAT,
			uins.count(), users, (unsigned char*)msg);
		delete[] users;
	}
	else
		seq=gg_send_message(Sess, GG_CLASS_CHAT, uins[0],
			(unsigned char*)msg);

	SocketNotifiers->checkWrite();

	kdebugf2();
	return seq;
}

int GaduProtocol::sendMessageRichText(const UinsList& uins, const char* msg, unsigned char* myLastFormats, int myLastFormatsLength)
{
	kdebugf();
	int seq;
	if(uins.count()>1)
	{
		UinType* users = new UinType[uins.count()];
		for (unsigned int i = 0; i < uins.count(); ++i)
			users[i] = uins[i];
		seq = gg_send_message_confer_richtext(Sess, GG_CLASS_CHAT,
				uins.count(), users, (unsigned char*)msg,
				myLastFormats, myLastFormatsLength);
		delete[] users;
	}
	else
		seq = gg_send_message_richtext(Sess, GG_CLASS_CHAT,
				uins[0], (unsigned char*)msg,
				myLastFormats, myLastFormatsLength);

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
			emit ackReceived(seq);
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, seq);
			emit messageQueued(seq, uin);
			emit messageAccepted(seq, uin);
			emit ackReceived(seq);
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

void GaduProtocol::sendUserList()
{
	kdebugf();
	UinType *uins;
	char *types;

	UserListSent = true;

	unsigned int j = 0;
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
		if ((*i).uin())
			++j;

	if (!j)
	{
		gg_notify_ex(Sess, NULL, NULL, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = (UinType *) malloc(j * sizeof(UinType));
	types = (char *) malloc(j * sizeof(char));

	j = 0;
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); ++i)
		if ((*i).uin() && !(*i).isAnonymous())
		{
			uins[j] = (*i).uin();
			if ((*i).offlineTo())
				types[j] = GG_USER_OFFLINE;
			else
				if ((*i).blocking())
					types[j] = GG_USER_BLOCKED;
				else
					types[j] = GG_USER_NORMAL;
			++j;
		}

	gg_notify_ex(Sess, uins, types, j);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	free(uins);
	free(types);
	kdebugf2();
}

bool GaduProtocol::sendImageRequest(UinType uin,int size,uint32_t crc32)
{
	kdebugf();
	int res = gg_image_request(Sess, uin, size, crc32);
	kdebugf2();
	return (res==0);
}

bool GaduProtocol::sendImage(UinType uin,const QString& file_name,uint32_t size,char* data)
{
	kdebugf();
	int res = gg_image_reply(Sess, uin, file_name.local8Bit().data(), data, size);
	kdebugf2();
	return (res==0);
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

	if (searchRecord.Uin.length())
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(searchRecord.Uin).data());
	if (searchRecord.FirstName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(searchRecord.FirstName).data());
	if (searchRecord.LastName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(searchRecord.LastName).data());
	if (searchRecord.NickName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(searchRecord.NickName).data());
	if (searchRecord.City.length())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(searchRecord.City).data());
	if (searchRecord.BirthYearFrom.length() && searchRecord.BirthYearTo.length())
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

	QString s = QString::number(searchRecord.FromUin);
	gg_pubdir50_add(req, GG_PUBDIR50_START, s.local8Bit());

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

	if (newData.First.length())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)(unicode2cp(newData.First).data()));
	if (newData.Last.length())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)(unicode2cp(newData.Last).data()));
	if (newData.Nick.length())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)(unicode2cp(newData.Nick).data()));
	if (newData.City.length())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)(unicode2cp(newData.City).data()));
	if (newData.Born.length())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)(unicode2cp(newData.Born).data()));
	if (newData.Gender)
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(newData.Gender).latin1());
	if (newData.FamilyName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(newData.FamilyName).data()));
	if (newData.FamilyCity.length())
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
	TokenSocketNotifiers *sn = new TokenSocketNotifiers(NULL, "token_socket_notifiers");
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

void GaduProtocol::remindPassword(UinType uin)
{
	kdebugf();

	Mode = RemindPassword;
	DataUin = uin;
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
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h, NULL, "pubdir_socket_notifiers");
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
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
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

	struct gg_http *h = gg_remind_passwd2(DataUin, unicode2cp(TokenId).data(),
		unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
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
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
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
	QString contacts(""), tmp;

	for (UserList::ConstIterator i = userList.begin(); i != userList.end(); ++i)
		if (!(*i).isAnonymous())
		{
			contacts += (*i).firstName();
			contacts += ";";
			contacts += (*i).lastName();
			contacts += ";";
			contacts += (*i).nickName();
			contacts += ";";
			contacts += (*i).altNick();
			contacts += ";";
			contacts += (*i).mobile();
			contacts += ";";
			tmp = (*i).group();
			tmp.replace(QRegExp(","), ";");
			contacts += tmp;
			contacts += ";";
			if ((*i).uin())
				contacts += QString::number((*i).uin());
			contacts += ";";
			contacts += (*i).email();
			contacts += ";0;;0;\r\n";
		}

	contacts.replace(QRegExp("(null)"), "");

	kdebugf2();
	return contacts;
}

void GaduProtocol::stringToUserList(QString &string, UserList& userList) const
{
	QTextStream stream(&string, IO_ReadOnly);
	streamToUserList(stream, userList);
}

void GaduProtocol::streamToUserList(QTextStream& stream, UserList& userList) const
{
	kdebugf();

	UserListElement e;
	QStringList sections, groupNames;
	QString line;
	int groups, i;
	bool ok;

	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));

	while (!stream.eof())
	{
		line = stream.readLine();
		sections = QStringList::split(";", line, true);

		// patrz kilka linijek ni¿ej...
		if (sections.count() < 12)
			continue;

		if (sections[6] == "0")
			sections[6].truncate(0);
		e.setFirstName(sections[0]);
		e.setLastName(sections[1]);
		e.setNickName(sections[2]);
		e.setAltNick(sections[3]);
		e.setMobile(sections[4]);

		// patrz kilka linijek wy¿ej
//		if (sections.count() >= 12)
			groups = sections.count() - 11;
//		else
//			groups = sections.count() - 7;

		groupNames.clear();
		for (i = 0; i < groups; ++i)
			groupNames.append(sections[5 + i]);
		e.setGroup(groupNames.join(","));
		e.setUin(sections[5 + groups].toUInt(&ok));
		if (!ok)
			e.setUin(0);

		e.setEmail(sections[6 + groups]);
		userList.addUser(e);
	}
	kdebugf2();
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
	char *dup = strdup(unicode2latin(contacts));

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

	ImportReply = "";

	bool success=(gg_userlist_request(Sess, GG_USERLIST_GET, NULL) != -1);
	if (!success)
	{
		UserList empty;
		emit userListImported(false, empty);
	}
	kdebugf2();
	return success;
}

void GaduProtocol::userListReceived(const struct gg_event *e)
{
	kdebugf();

	UserStatus oldStatus;
	int nr = 0;

	while (e->event.notify60[nr].uin)
	{
		if (!userlist.containsUin(e->event.notify60[nr].uin))
		{
			kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n",
				e->event.notify60[nr].uin);
			gg_remove_notify(Sess, e->event.notify60[nr].uin);
			++nr;
			continue;
		}

		UserListElement &user = userlist.byUin(e->event.notify60[nr].uin);

		user.ip().setAddress(ntohl(e->event.notify60[nr].remote_ip));
		user.refreshDnsName();
		user.setPort(e->event.notify60[nr].remote_port);
		user.setVersion(e->event.notify60[nr].version);
		user.setMaxImageSize(e->event.notify60[nr].image_size);

		oldStatus.setStatus(user.status());

		GaduStatus status;
		if (e->event.notify60[nr].descr)
			status.fromStatusNumber(e->event.notify60[nr].status,
				cp2unicode((unsigned char *)e->event.notify60[nr].descr));
		else
			status.fromStatusNumber(e->event.notify60[nr].status, "");
		user.status().setStatus(status);

		switch (e->event.notify60[nr].status)
		{
			case GG_STATUS_AVAIL:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went online\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went busy\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went offline\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BLOCKED:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d has blocked us\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went busy with description\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went offline with description\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went online with description\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went invisible with description\n",
					e->event.notify60[nr].uin);
				break;
			default:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unknown status for user %d: %d\n",
					e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
		}

		emit userStatusChanged(user, oldStatus, true);

		++nr;
	}

	emit userListChanged();

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

			UserList empty;
			emit userListImported(false, empty);
			return;
		}

		if (strlen(reply))
			ImportReply += cp2unicode((unsigned char *)reply);

		if (type == GG_USERLIST_GET_MORE_REPLY)
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "next portion\n");
			return;
		}

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", unicode2latin(ImportReply).data());

		UserList importedUserList;
		stringToUserList(ImportReply, importedUserList);

		emit userListImported(true, importedUserList);
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

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went %d (%s)\n", uin,
		status.toStatusNumber(), status.name().local8Bit().data());

	if (!userlist.containsUin(uin) || userlist.byUin(uin).isAnonymous())
	{
		// ignore!
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(Sess, uin);
		return;
	}

	UserListElement &user = userlist.byUin(uin);

	if (status.isOffline())
	{
		user.ip().setAddress((unsigned int)0);
		user.setPort(0);
		user.setVersion(0);
		user.setMaxImageSize(0);
	}
	else
	{
		user.ip().setAddress(ntohl(remote_ip));
		user.setPort(remote_port);
		user.setVersion(version);
		user.setMaxImageSize(image_size);
	}
	user.refreshDnsName();

	oldStatus.setStatus(user.status());
	user.status().setStatus(status);

	if (status != oldStatus)
		emit userStatusChanged(user, oldStatus);
	emit userListChanged();

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
	return gg_dcc_fill_file_info(d, filename.local8Bit());
}

struct gg_dcc* GaduProtocol::dccSocketCreate(UinType uin, uint16_t port)
{
	return gg_dcc_socket_create(uin, port);
}

struct gg_dcc* GaduProtocol::dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin)
{
	return gg_dcc_send_file(ip, port, my_uin, peer_uin);
}

struct gg_dcc* GaduProtocol::dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin)
{
	return gg_dcc_get_file(ip, port, my_uin, peer_uin);
}

struct gg_dcc* GaduProtocol::dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin)
{
	return gg_dcc_voice_chat(ip, port, my_uin, peer_uin);
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

void GaduProtocol::onCreateConfigDialog()
{
	kdebugf();

	QVGroupBox *g_proxy = ConfigDialog::getVGroupBox("Network", "Proxy server");
	QCheckBox *b_useproxy= ConfigDialog::getCheckBox("Network", "Use proxy server");

#ifdef HAVE_OPENSSL
	QCheckBox *b_tls= ConfigDialog::getCheckBox("Network", "Use TLSv1");
#endif
	QComboBox *cb_portselect= ConfigDialog::getComboBox("Network", "Default port to connect to servers");

	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	QCheckBox* b_defaultserver= ConfigDialog::getCheckBox("Network", "Use default servers");

	g_proxy->setEnabled(b_useproxy->isChecked());

//	((QHBox*)cb_portselect->parent())->setEnabled(!b_tls->isChecked());
	cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
	cb_portselect->setCurrentText(config_file.readEntry("Network", "DefaultPort", "8074"));

	serverbox->setEnabled(!b_defaultserver->isChecked());

	connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));

	kdebugf2();
}

void GaduProtocol::onDestroyConfigDialog()
{
	kdebugf();

	QComboBox *cb_portselect=ConfigDialog::getComboBox("Network", "Default port to connect to servers");
	config_file.writeEntry("Network","DefaultPort",cb_portselect->currentText());

	QLineEdit *e_servers=ConfigDialog::getLineEdit("Network", "IP addresses:", "server");

	QStringList tmpservers,server;
	QValueList<QHostAddress> servers;
	QHostAddress ip;
	bool ipok;
	unsigned int i;

	tmpservers = QStringList::split(";", e_servers->text());
	for (i = 0; i < tmpservers.count(); ++i)
	{
		ipok = ip.setAddress(tmpservers[i]);
		if (!ipok)
			break;
		servers.append(ip);
		server.append(ip.toString());
	}
	config_file.writeEntry("Network","Server",server.join(";"));
	ConfigServers = servers;
	ServerNr = 0;

	if (!ip.setAddress(config_file.readEntry("Network", "ProxyHost")))
	{
		MessageBox::msg(tr("Cannot set proxy IP address. Did you put hostname instead of IP address?"));
		config_file.writeEntry("Network","ProxyHost","0.0.0.0");
	}

	/* and now, save it */
	userlist.writeToFile();
	//
	kdebugf2();
}

void GaduProtocol::ifDefServerEnabled(bool value)
{
	kdebugf();
	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	serverbox->setEnabled(!value);
	kdebugf2();
}

void GaduProtocol::useTlsEnabled(bool value)
{
#ifdef HAVE_OPENSSL
	kdebugf();
	QHBox *box_portselect=(QHBox*)(ConfigDialog::getComboBox("Network", "Default port to connect to servers")->parent());
	box_portselect->setEnabled(!value);
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

	QString add = (hasDescription ? "WithDescription" : "");
	add.append(mobile ? (!hasDescription) ? "WithMobile" : "Mobile" : "");

	switch (stat)
	{
		case Online:
			return icons_manager.loadIcon(QString("Online").append(add));
		case Busy:
			return icons_manager.loadIcon(QString("Busy").append(add));
		case Invisible:
			return icons_manager.loadIcon(QString("Invisible").append(add));
		case Blocking:
			return icons_manager.loadIcon(QString("Blocking"));
		default:
			return icons_manager.loadIcon(QString("Offline").append(add));
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
	Description = "";

	switch (statusNumber)
	{
		case GG_STATUS_AVAIL:
		case GG_STATUS_AVAIL_DESCR:
			Stat = Online;
			break;

		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			Stat = Busy;
			break;

		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
		case GG_STATUS_INVISIBLE_DESCR:
			Stat = Invisible;
			break;

		case GG_STATUS_BLOCKED:
			Stat = Blocking;
			break;

		case GG_STATUS_NOT_AVAIL:
		case GG_STATUS_NOT_AVAIL_DESCR:
		default:
			Stat = Offline;
			break;
	}

	if ((statusNumber == GG_STATUS_AVAIL_DESCR) ||
	    (statusNumber == GG_STATUS_BUSY_DESCR) ||
	    (statusNumber == GG_STATUS_INVISIBLE_DESCR) ||
	    (statusNumber == GG_STATUS_NOT_AVAIL_DESCR))
		Description = description;

}

GaduProtocol* gadu;
