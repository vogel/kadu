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
int last_read_event = -1;
unsigned int server_nr = 0;
bool timeout_connected = true;
QTimer* pingtimer;
QValueList<QHostAddress> config_servers;
bool i_wanna_be_invisible = true;

QValueList<QHostAddress> gg_servers;
const char *gg_servers_ip[7] = {"217.17.41.82", "217.17.41.83", "217.17.41.84", "217.17.41.85",
	"217.17.41.86", "217.17.41.87", "217.17.41.88"};

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
};

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
}
/* SocketNotifiers */

SocketNotifiers::SocketNotifiers(int fd)
{
	kdebugf();

	Fd = fd;
}

SocketNotifiers::~SocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();
}

void SocketNotifiers::start()
{
	createSocketNotifiers();
}

void SocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	Snr = new QSocketNotifier(Fd, QSocketNotifier::Read);
	connect(Snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	Snw = new QSocketNotifier(Fd, QSocketNotifier::Write);
	connect(Snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

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

}

void SocketNotifiers::recreateSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();
	createSocketNotifiers();
}

/* PubdirSocketNotifiers */

PubdirSocketNotifiers::PubdirSocketNotifiers(struct gg_http *h)
	: SocketNotifiers(h->fd)
{
	kdebugf();
	H = h;
}

PubdirSocketNotifiers::~PubdirSocketNotifiers()
{
	deleteSocketNotifiers();
	kdebugf();
}

void PubdirSocketNotifiers::dataReceived()
{
	kdebugf();

	if (H->check & GG_CHECK_READ)
		socketEvent();
}

void PubdirSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();
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
			kdebug("PubdirSocketNotifiers::socketEvent(): changing QSocketNotifiers\n");
			recreateSocketNotifiers();

			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebug("PubdirSocketNotifiers::socketEvent(): error!\n");
			deleteSocketNotifiers();
			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			kdebug("PubdirSocketNotifiers::socketEvent(): success!\n");
			deleteSocketNotifiers();

			if (p->success)
				emit done(true, H);
			else
			{
				kdebug("PubdirSocketNotifiers::socketEvent(): error!\n");
				emit done(false, H);
			}
			gg_pubdir_free(H);
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
	}
}

/* DccSocketNotifier */

DccSocketNotifiers::DccSocketNotifiers(struct gg_dcc *d)
	: SocketNotifiers(d->fd)
{
	kdebugf();
	D = d;
}

DccSocketNotifiers::~DccSocketNotifiers()
{
}

void DccSocketNotifiers::dataReceived()
{
	kdebugf();

	socketEvent();
}

void DccSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (D->check & GG_CHECK_WRITE)
		socketEvent();
}

void DccSocketNotifiers::socketEvent()
{
	kdebugf();
	kdebug("FIXME: DccSocketNotifiers::socketEvent(): add some code here\n");
}

/* GaduProtocol */

void GaduProtocol::initModule()
{
	gadu = new GaduProtocol();
	
	QHostAddress ip;
	for (int i = 0; i < 7; i++)
	{
		ip.setAddress(QString(gg_servers_ip[i]));
		gg_servers.append(ip);
	}

//	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ResendUserlist"), tr("Resend &userlist"), gadu, SLOT(sendUserList()),0,-1,2);
}

GaduProtocol::GaduProtocol() : QObject()
{
	kdebugf();
	QObject::connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
	QObject::connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
}

void GaduProtocol::setStatus(int status)
{
	kdebugf();
	kdebug("GaduProtocol::setStatus(): setting status: %d\n",
		status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus")));

	status &= ~GG_STATUS_FRIENDS_MASK;
	i_wanna_be_invisible = (status == GG_STATUS_INVISIBLE) || (status == GG_STATUS_INVISIBLE_DESCR);
	
	//emit changingStatus();

	if (socket_active)
		changeStatus(status);
	else
		connect(status);

	//emit statusChanged(int);

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

void GaduProtocol::connect(int status)
{
	kdebugf();

	bool with_description = ifStatusWithDescription(status);

	memset(&loginparams, 0, sizeof(loginparams));
	loginparams.async = 1;

	// maksymalny rozmiar grafiki w kb
	loginparams.image_size = config_file.readNumEntry("Chat", "MaxImageSize", 20);

	// TODO proteza
	emit connecting();
	// tutaj przenie¶æ kupê kodu z kadu.cpp
	
	loginparams.status = status | (GG_STATUS_FRIENDS_MASK * config_file.readBoolEntry("General", "PrivateStatus"));

	if (with_description)
		loginparams.status_descr = strdup((const char *)unicode2cp(own_description));

	loginparams.password = strdup((const char *)unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	char *tmp = strdup((const char *)unicode2latin(pwHash(config_file.readEntry("General", "Password"))));
	kdebug("GaduProtocol::connect(): password = %s\n", tmp);
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
		loginparams.server_addr = htonl(config_servers[server_nr].ip4Addr());
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
		server_nr++;
		if (server_nr >= config_servers.count())
			server_nr = 0;
	}
	else
	{
		if (server_nr)
		{
			loginparams.server_addr = htonl(gg_servers[server_nr - 1].ip4Addr());
			loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");
		}			
		else
		{
			loginparams.server_addr = 0;
			loginparams.server_port = 0;
		}
		server_nr++;
		if (server_nr > gg_servers.count())
			server_nr = 0;
	}
	
//	polaczenia TLS z serwerami GG na razie nie dzialaja
//	loginparams.tls = config_file.readBoolEntry("Network", "UseTLS");
	loginparams.tls = 0;
	loginparams.client_version = GG_DEFAULT_CLIENT_VERSION;
	loginparams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	if (loginparams.tls)
	{
		kdebug("Kadu::setStatus(): using TLS\n");
		loginparams.server_port = 0;
		if (config_file.readBoolEntry("Network", "isDefServers"))
			loginparams.server_addr = 0;
		loginparams.server_port = 443;
	}
	else
		loginparams.server_port = config_file.readNumEntry("Network", "DefaultPort");

	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(&event_manager, SLOT(connectionTimeoutTimerSlot()));
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
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this);
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));
	}
	else
		emit disconnectNetwork();

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
	unsigned int i, j;

	for (i = 0, j = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			j++;

	if (!j) {
		gg_notify_ex(sess, NULL, NULL, 0);
		kdebug("send_userlist(): Userlist is empty\n");
		return;
		}

	uins = (UinType *) malloc(j * sizeof(UinType));
	types = (char *) malloc(j * sizeof(char));

	for (i = 0, j = 0; i < userlist.count(); i++)
		if (userlist[i].uin && !userlist[i].anonymous) {
			uins[j] = userlist[i].uin;
			if (userlist[i].offline_to_user)
				types[j] = GG_USER_OFFLINE;
			else
				if (userlist[i].blocking)
					types[j] = GG_USER_BLOCKED;
				else
					types[j] = GG_USER_NORMAL;
			j++;
			}

	/** we were popping up sometimes, so let's keep the server informed **/
	if (i_wanna_be_invisible)
		gg_change_status(sess, GG_STATUS_INVISIBLE);

	gg_notify_ex(sess, uins, types, j);
	kdebug("send_userlist(): Userlist sent\n");

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
	searchRecord.FromUin = 0;
	searchNextInPubdir(searchRecord);
}

void GaduProtocol::searchNextInPubdir(SearchRecord& searchRecord) {
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
}

void GaduProtocol::newResults(gg_pubdir50_t res)
{
	kdebugf();
	int count, fromUin;
	SearchResult searchResult;
	SearchResults searchResults;

	count = gg_pubdir50_count(res);

	kdebug("GaduProtocol::newResults(): found %d results\n", count);

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
}

/* rejestrowanie u¿ytkownika */

bool GaduProtocol::doRegister(QString& mail, QString& password, QString& token_id, QString& token_value)
{
	struct gg_http *h = gg_register3(unicode2cp(mail).data(), unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		QObject::connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(registerDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit registered(false, 0);
	return (h!=NULL);
}

void GaduProtocol::registerDone(bool ok, struct gg_http *h)
{
	if (ok)
		emit registered(true, ((struct gg_pubdir *)h->data)->uin);
	else
		emit registered(false, 0);
}

/* wyrejestrowywanie u¿ytkownika */

bool GaduProtocol::doUnregister(UinType uin, QString &password, QString& token_id, QString& token_value)
{
	struct gg_http* h = gg_unregister3(uin, unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		QObject::connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(unregisterDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit unregistered(false);
	return (h!=NULL);
}

void GaduProtocol::unregisterDone(bool ok, struct gg_http *)
{
	emit unregistered(ok);
}

/* przypomnienie hasla */

bool GaduProtocol::doRemind(UinType uin, QString& token_id, QString& token_value)
{
	kdebugf();

	struct gg_http *h = gg_remind_passwd2(uin, unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		PubdirSocketNotifiers *sn = new PubdirSocketNotifiers(h);
		QObject::connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(remindDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit reminded(false);
	return (h!=NULL);
}

void GaduProtocol::remindDone(bool ok, struct gg_http *)
{
	emit reminded(ok);
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
		QObject::connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(changePasswordDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit passwordChanged(false);
	return (h!=NULL);
}

void GaduProtocol::changePasswordDone(bool ok, struct gg_http *)
{
	emit passwordChanged(ok);
}

/* lista u¿ytkowników */

QString GaduProtocol::userListToString(const UserList& userList) const
{
	kdebugf();
	QString contacts(""), tmp;

	for (unsigned int i = 0; i < userList.count(); i++)
		if (!userList[i].anonymous)
		{
			contacts += userList[i].first_name;
			contacts += ";";
			contacts += userList[i].last_name;
			contacts += ";";
			contacts += userList[i].nickname;
			contacts += ";";
			contacts += userList[i].altnick;
			contacts += ";";
			contacts += userList[i].mobile;
			contacts += ";";
			tmp = userList[i].group();
			tmp.replace(QRegExp(","), ";");
			contacts += tmp;
			contacts += ";";
			if (userList[i].uin)
				contacts += QString::number(userList[i].uin);
			contacts += ";";
			contacts += userList[i].email;
			contacts += ";0;;0;\r\n";
		}

	contacts.replace(QRegExp("(null)"), "");

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
}

bool GaduProtocol::doExportUserList(const UserList& userList)
{
	kdebugf();

	QString contacts = userListToString(userList);
	char *dup = strdup(unicode2latin(contacts));

	kdebug("GaduProtocol::exportUserList():\n%s\n", dup);
	free(dup);

	userListClear = false;

	dup = strdup(unicode2std(contacts));
	if (gg_userlist_request(sess, GG_USERLIST_PUT, dup) == -1)
	{
		free(dup);
		kdebug("GaduProtocol:: gg_userlist_put() failed\n");
		emit userListExported(false);
		return false;
	}
	else
	{
		free(dup);
		return true;
	}
}

bool GaduProtocol::doClearUserList()
{
	kdebugf();

	userListClear = true;

	char *dup = "";
	if (gg_userlist_request(sess, GG_USERLIST_PUT, dup) == -1)
	{
		kdebug("GaduProtocol:: gg_userlist_out() failed\n");
		emit userListCleared(false);
		return false;
	}
	else
		return true;
}

bool GaduProtocol::doImportUserList()
{
	kdebugf();

	importReply = "";

	if (gg_userlist_request(sess, GG_USERLIST_GET, NULL) == -1)
	{
		UserList empty;
		emit userListImported(false, empty);
		return false;
	}
	else
		return true;
}

void GaduProtocol::userListReplyReceived(char type, char *reply)
{
	kdebugf();

	if (type == GG_USERLIST_PUT_REPLY)
	{
		kdebug("GaduProtocol::userlistReplyReceived(): put\n");
		kdebug("GaduProtocol::userlistReplyReceived(): Done\n");

		if (userListClear)
			emit userListCleared(true);
		else
			emit userListExported(true);

	}
	else if ((type == GG_USERLIST_GET_REPLY) || (type == GG_USERLIST_GET_MORE_REPLY))
	{
		kdebug("GaduProtocol::userlistReplyReceived(): get\n");

		if (!reply)
		{
			kdebug("GaduProtocol::userlistReplyReceived(): error!\n");

			UserList empty;
			emit userListImported(false, empty);
			return;
		}

		if (strlen(reply))
			importReply += cp2unicode((unsigned char *)reply);

		if (type == GG_USERLIST_GET_MORE_REPLY)
		{
			kdebug("GaduProtocol::userListReplyReceived(): next portion\n");
			return;
		}

		kdebug("GaduProtocol::userListReplyReceived(): Done.\n");
		kdebug("GaduProtocol::userListReplyReceived()\n%s\n",
			unicode2latin(importReply).data());

		//QTextStream stream(&importReply, IO_ReadOnly);
		UserList importedUserList;
		stringToUserList(importReply, importedUserList);

		emit userListImported(true, importedUserList);
	}
}

GaduProtocol* gadu;

