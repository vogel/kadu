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
#include "events.h"
#include "kadu.h"
#include "chat.h"
#include "ignore.h"
#include "dcc.h"

#include <netinet/in.h>

#include <qregexp.h>
#include <qsocketnotifier.h>
#include <qtextcodec.h>

#include <stdlib.h>

struct gg_session* sess = NULL;
struct gg_login_params loginparams;

QSocketNotifier* kadusnr = NULL;
QSocketNotifier* kadusnw = NULL;

bool userlist_sent = false;
bool socket_active = false;
unsigned int server_nr = 0;
QValueList<QHostAddress> config_servers;

QValueList<QHostAddress> gg_servers;
const char *gg_servers_ip[7] = {"217.17.41.82", "217.17.41.83", "217.17.41.84", "217.17.41.85",
	"217.17.41.86", "217.17.41.87", "217.17.41.88"};

bool UinsList::equals(const UinsList &uins) const
{
	if (count() != uins.count())
		return false;
	for (UinsList::const_iterator i = begin(); i != end(); i++)
		if(!uins.contains(*i))
			return false;
	return true;
}

UinsList::UinsList() {
}

void UinsList::sort() {
	qHeapSort(*this);
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
	Status = copyFrom.Status;
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
		Status = atoi(status) & 127;
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

SocketNotifiers::SocketNotifiers(int fd)
{
	kdebugf();

	Fd = fd;

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

PubdirSocketNotifiers::PubdirSocketNotifiers(struct gg_http *h)
	: SocketNotifiers(h->fd)
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
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "PubdirSocketNotifiers::socketEvent(): changing QSocketNotifiers\n");
			recreateSocketNotifiers();

			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "PubdirSocketNotifiers::socketEvent(): error!\n");
			deleteSocketNotifiers();
			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "PubdirSocketNotifiers::socketEvent(): success!\n");
			deleteSocketNotifiers();

			if (p->success)
				emit done(true, H);
			else
			{
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "PubdirSocketNotifiers::socketEvent(): error!\n");
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

/* DccSocketNotifier */

DccSocketNotifiers::DccSocketNotifiers(struct gg_dcc *d)
	: SocketNotifiers(d->fd)
{
	kdebugf();
	D = d;
	kdebugf2();
}

DccSocketNotifiers::~DccSocketNotifiers()
{
}

void DccSocketNotifiers::dataReceived()
{
	kdebugf();

	socketEvent();
	kdebugf2();
}

void DccSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (D->check & GG_CHECK_WRITE)
		socketEvent();
	kdebugf2();
}

void DccSocketNotifiers::socketEvent()
{
	kdebugf();
	kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "FIXME: DccSocketNotifiers::socketEvent(): add some code here\n");
}

/* GaduSocketNotifiers */

GaduSocketNotifiers::GaduSocketNotifiers()
{
	kdebugf();
	kdebugf2();
}

GaduSocketNotifiers::~GaduSocketNotifiers()
{
}

void GaduSocketNotifiers::eventHandler(gg_session *sess)
{
	kdebugf();
	static int calls = 0;

	calls++;
	if (calls > 1)
		kdebugm(KDEBUG_WARNING, "************* GaduSocketNotifiers::eventHandler(): Recursive eventHandler calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(sess)))
	{
		emit error(ConnectionUnknow);
		gg_free_event(e);
		calls--;
		return;
	}

	if (sess->state == GG_STATE_CONNECTING_HUB || sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduSocketNotifiers::eventHandler(): changing QSocketNotifiers.\n");

		kadusnw->setEnabled(false);
		delete kadusnw;

		kadusnr->setEnabled(false);
		delete kadusnr;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this);
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this);
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));
	}

	switch (sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Waiting for hash key\n");
			ConnectionTimeoutTimer::off();
			break;
		case GG_STATE_READING_REPLY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Sending key\n");
			ConnectionTimeoutTimer::off();
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
			if (config_file.readBoolEntry("Network", "AllowDCC") && !isIgnored(uins))
				emit dccConnectionReceived(userlist.byUin(e->event.msg.sender));
		}
		else
		{
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventHandler(): %d\n", e->event.msg.recipients_count);
			if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT)
			{
				uins.append(e->event.msg.sender);
				for (int i = 0; i < e->event.msg.recipients_count; i++)
					uins.append(e->event.msg.recipients[i]);
			}
			else
				uins.append(e->event.msg.sender);
			QCString msg((char*)e->event.msg.message);
			QByteArray formats;
			formats.duplicate((const char*)e->event.msg.formats, e->event.msg.formats_length);
			emit messageReceived(e->event.msg.msgclass, uins, msg,
				e->event.msg.time, formats);
		}
	}

	if (e->type == GG_EVENT_IMAGE_REQUEST)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image request received\n");
		emit imageRequestReceived(
			e->event.image_request.sender,
			e->event.image_request.size,
			e->event.image_request.crc32);
	}

	if (e->type == GG_EVENT_IMAGE_REPLY)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image reply received\n");
		emit imageReceived(
			e->event.image_reply.sender,
			e->event.image_reply.size,
			e->event.image_reply.crc32,
			e->event.image_reply.filename,
			e->event.image_reply.image);
	}

	if (e->type == GG_EVENT_STATUS60 || e->type == GG_EVENT_STATUS)
		emit userStatusChanged(e);

	if (e->type == GG_EVENT_ACK)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): message reached %d (seq %d)\n",
			e->event.ack.recipient, e->event.ack.seq);
		emit ackReceived(e->event.ack.seq);
	}

	if (e->type == GG_EVENT_NOTIFY60)
		emit userlistReceived(e);

	if (e->type == GG_EVENT_PUBDIR50_SEARCH_REPLY
		|| e->type == GG_EVENT_PUBDIR50_READ || e->type == GG_EVENT_PUBDIR50_WRITE)
		emit pubdirReplyReceived(e->event.pubdir50);

	if (e->type == GG_EVENT_USERLIST)
		emit userlistReplyReceived(e->event.userlist.type, e->event.userlist.reply);

	if (e->type == GG_EVENT_CONN_SUCCESS)
		emit connected();

	if (e->type == GG_EVENT_CONN_FAILED)
		connectionFailed(e->event.failure);

	if (e->type == GG_EVENT_DISCONNECT)
		emit disconnected();

	if (socket_active)
	{
		if (sess->state == GG_STATE_IDLE && userlist_sent)
		{
			socket_active = false;
			UserBox::all_changeAllToInactive();
			emit error(ConnectionUnknow);
		}
		else
			if (sess->check & GG_CHECK_WRITE)
				kadusnw->setEnabled(true);
	}

	gg_free_event(e);
	calls--;
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
	}

	emit error(err);
	kdebugf2();
}

/* GaduProtocol */

void GaduProtocol::initModule()
{
	kdebugf();
	gadu = new GaduProtocol(kadu, "gadu");

	QHostAddress ip;
	for (int i = 0; i < 7; i++)
	{
		ip.setAddress(QString(gg_servers_ip[i]));
		gg_servers.append(ip);
	}

	gg_proxy_host = NULL;
	gg_proxy_username = NULL;
	gg_proxy_password = NULL;

//	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ResendUserlist"), tr("Resend &userlist"), gadu, SLOT(sendUserList()),0,-1,2);
	kdebugf2();
}

GaduProtocol::GaduProtocol(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

	SocketNotifiers = new GaduSocketNotifiers();
	PingTimer = NULL;
	ActiveServer = NULL;
	IWannaBeInvisible = true;

	connect(SocketNotifiers, SIGNAL(ackReceived(int)), this, SIGNAL(ackReceived(int)));
	connect(SocketNotifiers, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(SocketNotifiers, SIGNAL(dccConnectionReceived(const UserListElement &)),
		this, SLOT(dccConnectionReceived(const UserListElement &)));
	connect(SocketNotifiers, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
	connect(SocketNotifiers, SIGNAL(error(GaduError)), this, SLOT(errorSlot(GaduError)));
	connect(SocketNotifiers, SIGNAL(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)),
		this, SLOT(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)));
	connect(SocketNotifiers, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SLOT(imageRequestReceived(UinType, uint32_t, uint32_t)));
	connect(SocketNotifiers, SIGNAL(messageReceived(int, UinsList, QCString &, time_t, QByteArray &)),
		this, SLOT(messageReceived(int, UinsList, QCString &, time_t, QByteArray &)));
	connect(SocketNotifiers, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)), this, SLOT(newResults(gg_pubdir50_t)));
	connect(SocketNotifiers, SIGNAL(systemMessageReceived(QString &, QDateTime &, int, void *)),
		this, SLOT(systemMessageReceived(QString &, QDateTime &, int, void *)));
	connect(SocketNotifiers, SIGNAL(userlistReceived(struct gg_event *)),
		this, SLOT(userListReceived(struct gg_event *)));
	connect(SocketNotifiers, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
	connect(SocketNotifiers, SIGNAL(userStatusChanged(struct gg_event *)),
		this, SLOT(userStatusChanged(struct gg_event *)));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	delete SocketNotifiers;

	kdebugf2();
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
	server_nr = 0;
	PingTimer = new QTimer();
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(pingNetwork()));
	PingTimer->start(60000, TRUE);

	emit connected();
	emit statusChanged(loginparams.status & (~GG_STATUS_FRIENDS_MASK));

	kdebugf2();
}

void GaduProtocol::dccConnectionReceived(const UserListElement &sender)
{
	kdebugf();

	struct gg_dcc *dcc_new;
	FileDccSocket *dcc;
	if (DccSocket::count() < 8)
	{
		dcc_new = gg_dcc_get_file(htonl(sender.ip.ip4Addr()), sender.port, config_file.readNumEntry("General","UIN"), sender.uin);
		if (dcc_new)
		{
			dcc = new FileDccSocket(dcc_new);
			connect(dcc, SIGNAL(dccFinished(DccSocket*)), dcc_manager, SLOT(dccFinished(DccSocket*)));
			dcc->initializeNotifiers();
		}
	}

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

	if (kadusnw)
	{
		kadusnw->setEnabled(false);
		delete kadusnw;
		kadusnw = NULL;
	}

	if (kadusnr)
	{
		kadusnr->setEnabled(false);
		delete kadusnr;
		kadusnr = NULL;
	}

	if (dccsnr)
	{
		delete dccsnr;
		dccsnr = NULL;
	}

	if (dccsnw)
	{
		delete dccsnw;
		dccsnw = NULL;
	}

	if (dccsock)
	{
		gg_dcc_free(dccsock);
		dccsock = NULL;
		gg_dcc_ip = 0;
		gg_dcc_port = 0;
	}


	if (sess)
	{
		gg_logoff(sess);
		gg_free_session(sess);
		sess = NULL;
	}

	userlist_sent = false;

	for (UserList::Iterator i = userlist.begin(); i != userlist.end(); i++)
	{
		(*i).status = GG_STATUS_NOT_AVAIL;
		(*i).description = "";
	}

	chat_manager->refreshTitles();

//	own_description = QString::null;
	UserBox::all_refresh();

	socket_active = false;

	emit statusChanged(GG_STATUS_NOT_AVAIL);
	emit disconnected();
	kdebugf2();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();
	if (sess->state == GG_STATE_CONNECTING_HUB ||
		sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebug("Timeout, breaking connection\n");
		emit error(ConnectionTimeout);
		logout();
		login(RequestedStatusForLogin);
	}
	kdebugf2();
}

void GaduProtocol::errorSlot(GaduError err)
{
	kdebugf();

	disconnectedSlot();
	emit error(err);
	if(err != Disconnected)
		login(RequestedStatusForLogin);
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

void GaduProtocol::imageRequestReceived(UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugm(KDEBUG_INFO, QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(sender).arg(size).arg(crc32).local8Bit().data());

	gadu_images_manager.sendImage(sender,size,crc32);
}

void GaduProtocol::messageReceived(int msgclass, UinsList senders, QCString &msg, time_t time,
	QByteArray &formats)
{
/*
	sprawdzamy czy user jest na naszej liscie, jezeli nie to anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/
	if (userlist.byUinValue(senders[0]).anonymous && config_file.readBoolEntry("Chat","IgnoreAnonymousUsers"))
	{
		kdebugm(KDEBUG_INFO, "EventManager::messageReceivedSlot(): Ignored anonymous. %d is ignored\n", senders[0]);
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

	// wiadomosci systemowe maja sensers[0] = 0
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

	if(!userlist.containsUin(senders[0]))
		userlist.addAnonymous(senders[0]);

	kdebugm(KDEBUG_INFO, "eventRecvMsg(): Got message from %d saying \"%s\"\n",
			senders[0], (const char *)mesg.local8Bit());

	emit chatMsgReceived1(senders, mesg, time, grab);
	if(!grab)
		emit chatMsgReceived2(senders, mesg, time);
}

void GaduProtocol::pingNetwork()
{
	kdebugf();
	gg_ping(sess);
	PingTimer->start(60000, TRUE);
	kdebugf2();
}

void GaduProtocol::setStatus(int status)
{
	kdebugf();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setStatus(): setting status: %d\n",
		status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));

	status &= ~GG_STATUS_FRIENDS_MASK;
	IWannaBeInvisible = (status == GG_STATUS_INVISIBLE) || (status == GG_STATUS_INVISIBLE_DESCR);

	//emit changingStatus();

	if (socket_active)
		changeStatus(status);
	else
		login(status);

	//emit statusChanged(int);

	kdebugf2();
}

void GaduProtocol::systemMessageReceived(QString &message, QDateTime &time, int formats_length, void *formats)
{
	kdebugf();

	QString mesg = time.toString("hh:mm:ss (dd.MM.yyyy): ") + message;
	emit systemMessageReceived(mesg);

	kdebugf2();
}

void GaduProtocol::changeStatus(int status)
{
	kdebugf();

	QString sigDesc = QString::null;
	bool stop = false;
	bool with_description = ifStatusWithDescription(status);
	unsigned char *descr;

	if (with_description)
		sigDesc = own_description;

//	emit kadu->changingStatus(status, sigDesc, stop); //?? moze przjdzie TODO TOSEE
	if (stop)
		return;

	if (with_description)
	{
		descr = (unsigned char *)strdup(unicode2cp(own_description).data());
		if (status == GG_STATUS_NOT_AVAIL_DESCR)
			gg_change_status_descr(sess, status, (const char *)descr);
		else
			gg_change_status_descr(sess,
				status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")), (const char *)descr);
		free(descr);
	}
	else
	{
		gg_change_status(sess, status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));
		own_description = QString::null;
	}

	if (status != GG_STATUS_NOT_AVAIL && status != GG_STATUS_NOT_AVAIL_DESCR)
		loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus"));

	emit statusChanged(status);	// TODO wymaga rozwazenia

	kdebugf2();
}

void GaduProtocol::login(int status)
{
	kdebugf();
	RequestedStatusForLogin = status;

	emit connecting();

	bool with_description = ifStatusWithDescription(status);

	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;

	// maksymalny rozmiar grafiki w kb
	loginparams.image_size = config_file.readNumEntry("Chat", "MaxImageSize", 20);

	if (config_file.readBoolEntry("Network", "AllowDCC"))
		setupDcc();
	setupProxy();

	loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus"));

	if (with_description)
		loginparams.status_descr = strdup((const char *)unicode2cp(own_description));

	loginparams.password = strdup((const char *)unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	char *tmp = strdup((const char *)unicode2latin(pwHash(config_file.readEntry("General", "Password"))));
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::connect(): password = %s\n", tmp);
	free(tmp);

	loginparams.uin = (UinType)config_file.readNumEntry("General", "UIN");
	loginparams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	loginparams.last_sysmsg = config_file.readNumEntry("Global", "SystemMsgIndex");

	if (config_file.readBoolEntry("Network", "AllowDCC") && config_extip.ip4Addr() && config_file.readNumEntry("Network", "ExternalPort") > 1023)
	{
		loginparams.external_addr = htonl(config_extip.ip4Addr());
		loginparams.external_port = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		loginparams.external_addr = 0;
		loginparams.external_port = 0;
	}

	if (config_servers.count() && !config_file.readBoolEntry("Network", "isDefServers") && config_servers[server_nr].ip4Addr())
	{
		ActiveServer = &config_servers[server_nr];
		server_nr++;
		if (server_nr >= config_servers.count())
			server_nr = 0;
	}
	else
	{
		if (server_nr)
			ActiveServer = &gg_servers[server_nr - 1];
		else
			ActiveServer = NULL;
		server_nr++;
		if (server_nr > gg_servers.count())
			server_nr = 0;
	}

	if (ActiveServer != NULL)
	{
		loginparams.server_addr = htonl(ActiveServer->ip4Addr());
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
	}
	else
	{
		loginparams.server_addr = 0;
		loginparams.server_port = 0;
	}

//	polaczenia TLS z serwerami GG na razie nie dzialaja
//	loginparams.tls = config_file.readBoolEntry("Network", "UseTLS");
	loginparams.tls = 0;
	loginparams.client_version = GG_DEFAULT_CLIENT_VERSION;
	loginparams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	if (loginparams.tls)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::login((): using TLS\n");
		loginparams.server_port = 0;
		if (config_file.readBoolEntry("Network", "isDefServers"))
			loginparams.server_addr = 0;
		loginparams.server_port = 443;
	}
	else
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");

	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(this, SLOT(connectionTimeoutTimerSlot()));
	sess = gg_login(&loginparams);
//	free(loginparams.client_version);
	free(loginparams.password);
	if (loginparams.status_descr)
		free(loginparams.status_descr);

	AutoConnectionTimer::off();

	if (sess)
	{
		socket_active = true;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this);
		connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this);
		connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));
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

		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setupProxy(): gg_proxy_host = %s\n", gg_proxy_host);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setupProxy(): gg_proxy_port = %d\n", gg_proxy_port);

		if (config_file.readEntry("Network", "ProxyUser").length())
		{
			gg_proxy_username = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyUser")).data());
			gg_proxy_password = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyPassword")).data());
		}
	}

	kdebugf2();
}

void GaduProtocol::setupDcc()
{
	kdebugf();

	QHostAddress dccIp;

	if (!config_dccip.ip4Addr())
		dccIp.setAddress("255.255.255.255");
	else
		dccIp = config_dccip;

	dccsock = gg_dcc_socket_create(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550));

	if (!dccsock)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setupDcc(): Couldn't bind DCC socket.\n");
		gg_dcc_free(dccsock);

		emit dccSetupFailed();
		return;
	}

	gg_dcc_ip = htonl(dccIp.ip4Addr());
	gg_dcc_port = dccsock->port;

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol:setupDcc() DCC_IP=%s DCC_PORT=%d\n", dccIp.toString().latin1(), dccsock->port);

	dccsnr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, kadu);
	QObject::connect(dccsnr, SIGNAL(activated(int)), dcc_manager, SLOT(dccReceived()));

	dccsnw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, kadu);
	QObject::connect(dccsnw, SIGNAL(activated(int)), dcc_manager, SLOT(dccSent()));

	kdebugf2();
}

int GaduProtocol::sendMessage(const UinsList& uins,const char* msg)
{
	kdebugf();
	int seq;
	if(uins.count()>1)
	{
		UinType* users = new UinType[uins.count()];
		for (unsigned int i = 0; i < uins.count(); i++)
			users[i] = uins[i];
		seq=gg_send_message_confer(sess, GG_CLASS_CHAT,
			uins.count(), users, (unsigned char*)msg);
		delete[] users;
	}
	else
		seq=gg_send_message(sess, GG_CLASS_CHAT, uins[0],
			(unsigned char*)msg);
	kdebugf2();
	return seq;
}

int GaduProtocol::sendMessageRichText(const UinsList& uins,const char* msg,unsigned char* myLastFormats,int myLastFormatsLength)
{
	kdebugf();
	int seq;
	if(uins.count()>1)
	{
		UinType* users = new UinType[uins.count()];
		for (unsigned int i = 0; i < uins.count(); i++)
			users[i] = uins[i];
		seq = gg_send_message_confer_richtext(sess, GG_CLASS_CHAT,
				uins.count(), users, (unsigned char*)msg,
				myLastFormats, myLastFormatsLength);
		delete[] users;
	}
	else
		seq = gg_send_message_richtext(sess, GG_CLASS_CHAT,
				uins[0], (unsigned char*)msg,
				myLastFormats, myLastFormatsLength);
	kdebugf2();
	return seq;
}

void GaduProtocol::sendUserList()
{
	kdebugf();
	UinType *uins;
	char *types;

	userlist_sent = true;

	unsigned int j = 0;
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); i++)
		if ((*i).uin)
			j++;

	if (!j) {
		gg_notify_ex(sess, NULL, NULL, 0);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "send_userlist(): Userlist is empty\n");
		return;
		}

	uins = (UinType *) malloc(j * sizeof(UinType));
	types = (char *) malloc(j * sizeof(char));

	j = 0;
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); i++)
		if ((*i).uin && !(*i).anonymous)
		{
			uins[j] = (*i).uin;
			if ((*i).offline_to_user)
				types[j] = GG_USER_OFFLINE;
			else
				if ((*i).blocking)
					types[j] = GG_USER_BLOCKED;
				else
					types[j] = GG_USER_NORMAL;
			j++;
		}

	/** we were popping up sometimes, so let's keep the server informed **/
	if (IWannaBeInvisible)
		gg_change_status(sess, GG_STATUS_INVISIBLE);

	gg_notify_ex(sess, uins, types, j);
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "send_userlist(): Userlist sent\n");

	free(uins);
	free(types);
	kdebugf2();
}

bool GaduProtocol::sendImageRequest(UinType uin,int size,uint32_t crc32)
{
	kdebugf();
	int res = gg_image_request(sess, uin, size, crc32);
	kdebugf2();
	return (res==0);
}

bool GaduProtocol::sendImage(UinType uin,const QString& file_name,uint32_t size,char* data)
{
	kdebugf();
	int res = gg_image_reply(sess, uin, file_name.local8Bit().data(), data, size);
	kdebugf2();
	return (res==0);
}

/* wyszukiwanie w katalogu publicznym */

void GaduProtocol::searchInPubdir(SearchRecord& searchRecord) {
	kdebugf();
	searchRecord.FromUin = 0;
	searchNextInPubdir(searchRecord);
	kdebugf2();
}

void GaduProtocol::searchNextInPubdir(SearchRecord& searchRecord) {
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

	searchRecord.Seq = gg_pubdir50(sess, req);
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

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::newResults(): found %d results\n", count);

	for (int i = 0; i < count; i++)
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
	searchRecord.Seq = gg_pubdir50(sess, req);
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

	searchRecord.Seq = gg_pubdir50(sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

/* rejestrowanie u¿ytkownika */

bool GaduProtocol::doRegister(QString& mail, QString& password, QString& token_id, QString& token_value)
{
	kdebugf();
	struct gg_http *h = gg_register3(unicode2cp(mail).data(), unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(registerDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit registered(false, 0);
	kdebugf2();
	return (h!=NULL);
}

void GaduProtocol::registerDone(bool ok, struct gg_http *h)
{
	kdebugf();
	if (ok)
		emit registered(true, ((struct gg_pubdir *)h->data)->uin);
	else
		emit registered(false, 0);
	kdebugf2();
}

/* wyrejestrowywanie u¿ytkownika */

bool GaduProtocol::doUnregister(UinType uin, QString &password, QString& token_id, QString& token_value)
{
	kdebugf();
	struct gg_http* h = gg_unregister3(uin, unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(unregisterDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit unregistered(false);
	kdebugf2();
	return (h!=NULL);
}

void GaduProtocol::unregisterDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit unregistered(ok);
	kdebugf2();
}

/* przypomnienie hasla */

bool GaduProtocol::doRemind(UinType uin, QString& token_id, QString& token_value)
{
	kdebugf();

	struct gg_http *h = gg_remind_passwd2(uin, unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(remindDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit reminded(false);
	kdebugf2();
	return (h!=NULL);
}

void GaduProtocol::remindDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit reminded(ok);
	kdebugf2();
}

/* zmiana has³a */

bool GaduProtocol::doChangePassword(UinType uin, QString& mail, QString& password, QString& new_password, QString& token_id, QString& token_val)
{
	kdebugf();

	struct gg_http *h = gg_change_passwd4(uin, unicode2cp(mail).data(), unicode2cp(password).data(),
			unicode2cp(new_password).data(), unicode2cp(token_id).data(), unicode2cp(token_val).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(changePasswordDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit passwordChanged(false);
	kdebugf2();
	return (h!=NULL);
}

void GaduProtocol::changePasswordDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit passwordChanged(ok);
	kdebugf2();
}

/* lista u¿ytkowników */

QString GaduProtocol::userListToString(const UserList& userList) const
{
	kdebugf();
	QString contacts(""), tmp;

	for (UserList::ConstIterator i = userList.begin(); i != userList.end(); i++)
		if (!(*i).anonymous)
		{
			contacts += (*i).first_name;
			contacts += ";";
			contacts += (*i).last_name;
			contacts += ";";
			contacts += (*i).nickname;
			contacts += ";";
			contacts += (*i).altnick;
			contacts += ";";
			contacts += (*i).mobile;
			contacts += ";";
			tmp = (*i).group();
			tmp.replace(QRegExp(","), ";");
			contacts += tmp;
			contacts += ";";
			if ((*i).uin)
				contacts += QString::number((*i).uin);
			contacts += ";";
			contacts += (*i).email;
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
		if (sections.count() < 12)
			continue;
		if (sections[6] == "0")
			sections[6].truncate(0);
		e.first_name = sections[0];
		e.last_name = sections[1];
		e.nickname = sections[2];
		e.altnick = sections[3];
		e.mobile = sections[4];
		if (sections.count() >= 12)
			groups = sections.count() - 11;
		else
			groups = sections.count() - 7;
		groupNames.clear();
		for (i = 0; i < groups; i++)
			groupNames.append(sections[5 + i]);
		e.setGroup(groupNames.join(","));
		e.uin = sections[5 + groups].toUInt(&ok);
		if (!ok)
			e.uin = 0;
		e.description = QString::null;
		e.email = sections[6 + groups];
		userList.addUser(e);
	}
	kdebugf2();
}

bool GaduProtocol::doExportUserList(const UserList& userList)
{
	kdebugf();

	QString contacts = userListToString(userList);
	char *dup = strdup(unicode2latin(contacts));

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::exportUserList():\n%s\n", dup);
	free(dup);

	userListClear = false;

	dup = strdup(unicode2std(contacts));
	bool success=(gg_userlist_request(sess, GG_USERLIST_PUT, dup)!=-1);
	if (!success)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol:: gg_userlist_put() failed\n");
		emit userListExported(false);
	}
	free(dup);
	kdebugf2();
	return success;
}

bool GaduProtocol::doClearUserList()
{
	kdebugf();

	userListClear = true;

	char *dup = "";
	bool success=(gg_userlist_request(sess, GG_USERLIST_PUT, dup) != -1);
	if (!success)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol:: gg_userlist_out() failed\n");
		emit userListCleared(false);
	}
	kdebugf2();
	return success;
}

bool GaduProtocol::doImportUserList()
{
	kdebugf();

	importReply = "";

	bool success=(gg_userlist_request(sess, GG_USERLIST_GET, NULL) != -1);
	if (!success)
	{
		UserList empty;
		emit userListImported(false, empty);
	}
	kdebugf2();
	return success;
}

void GaduProtocol::userListReceived(struct gg_event *e)
{
	kdebugf();

	unsigned int oldStatus;
	int nr = 0;

	while (e->event.notify60[nr].uin)
	{
		if (!userlist.containsUin(e->event.notify60[nr].uin))
		{
			kdebugm(KDEBUG_INFO, "eventGotUserlist(): buddy %d not in list. Damned server!\n",
				e->event.notify60[nr].uin);
			gg_remove_notify(sess, e->event.notify60[nr].uin);
			nr++;
			continue;
		}

		UserListElement &user = userlist.byUin(e->event.notify60[nr].uin);

		user.ip.setAddress(ntohl(e->event.notify60[nr].remote_ip));
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = e->event.notify60[nr].remote_port;
		user.version = e->event.notify60[nr].version;
		user.image_size = e->event.notify60[nr].image_size;

		oldStatus = user.status;

		if (user.description)
			user.description.truncate(0);

		if (e->event.notify60[nr].descr)
			user.description.append(cp2unicode((unsigned char *)e->event.notify60[nr].descr));

		switch (e->event.notify60[nr].status)
		{
			case GG_STATUS_AVAIL:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went online\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went busy\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went offline\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BLOCKED:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d has blocked us\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went busy with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went offline with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went online with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went invisible with descr.\n",
					e->event.notify60[nr].uin);
				break;
			default:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): Unknown status for user %d: %d\n",
					e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
		}

		userlist.changeUserStatus(e->event.notify60[nr].uin, e->event.notify60[nr].status);
		emit userStatusChanged(user, oldStatus);

		nr++;
	}

	emit userListChanged();

	kdebugf2();
}

void GaduProtocol::userListReplyReceived(char type, char *reply)
{
	kdebugf();

	if (type == GG_USERLIST_PUT_REPLY)
	{
//		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userlistReplyReceived(): put\n");
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userlistReplyReceived(): Done\n");

		if (userListClear)
			emit userListCleared(true);
		else
			emit userListExported(true);

	}
	else if ((type == GG_USERLIST_GET_REPLY) || (type == GG_USERLIST_GET_MORE_REPLY))
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userlistReplyReceived(): get\n");

		if (!reply)
		{
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userlistReplyReceived(): error!\n");

			UserList empty;
			emit userListImported(false, empty);
			return;
		}

		if (strlen(reply))
			importReply += cp2unicode((unsigned char *)reply);

		if (type == GG_USERLIST_GET_MORE_REPLY)
		{
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userListReplyReceived(): next portion\n");
			return;
		}

//		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userListReplyReceived(): Done.\n");
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::userListReplyReceived()\n%s\n",
			unicode2latin(importReply).data());

		UserList importedUserList;
		stringToUserList(importReply, importedUserList);

		emit userListImported(true, importedUserList);
	}

	kdebugf2();
}

void GaduProtocol::userStatusChanged(struct gg_event *e)
{
	kdebugf();

	unsigned int oldStatus, status;
	uint32_t uin;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint8_t version;
	uint8_t image_size;
	char *descr;

	if (e->type == GG_EVENT_STATUS60)
	{
		uin = e->event.status60.uin;
		status = e->event.status60.status;
		descr = e->event.status60.descr;
		remote_ip = e->event.status60.remote_ip;
		remote_port = e->event.status60.remote_port;
		version = e->event.status60.version;
		image_size = e->event.status60.image_size;
	}
	else
	{
		uin = e->event.status.uin;
		status = e->event.status.status;
		descr = e->event.status.descr;
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventStatusChange(): User %d went %d\n", uin, status);
	UserListElement &user = userlist.byUin(uin);

	if (!userlist.containsUin(uin))
	{
		// ignore!
		kdebugm(KDEBUG_INFO, "eventStatusChange(): buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(sess, uin);
		return;
	}

	oldStatus = user.status;

	if (user.description)
		user.description.truncate(0);

//	if (ifStatusWithDescription(e->event.status.status)) {
	if (descr)
		user.description.append(cp2unicode((unsigned char *)descr));
	userlist.changeUserStatus(uin, status);

	if (user.status == GG_STATUS_NOT_AVAIL || user.status == GG_STATUS_NOT_AVAIL_DESCR)
	{
		user.ip.setAddress((unsigned int)0);
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = 0;
		user.version = 0;
		user.image_size = 0;
	}
	else
	{
		user.ip.setAddress(ntohl(remote_ip));
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = remote_port;
		user.version = version;
		user.image_size = image_size;
	}

	emit userStatusChanged(user, oldStatus);
	emit userListChanged();

	kdebugf2();
}

GaduProtocol* gadu;

