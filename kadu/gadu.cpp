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

#include <qsocketnotifier.h>
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

SearchResult::SearchResult()
{
}

SearchResult::SearchResult(const SearchResult& copyFrom)
{
	this->uin = copyFrom.uin;
	this->first = copyFrom.first;
	this->nick = copyFrom.nick;
	this->born = copyFrom.born;
	this->city = copyFrom.city;
	this->status = copyFrom.status;
}

void SearchResult::setData(const char *uin, const char *first, const char *nick, const char *born, const char *city, const char *status)
{
	this->uin = cp2unicode((unsigned char *)uin);
	this->first = cp2unicode((unsigned char *)first);
	this->nick = cp2unicode((unsigned char *)nick);
	this->born = cp2unicode((unsigned char *)born);
	this->city = cp2unicode((unsigned char *)city);
	this->status = atoi(status) & 127;
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
	this->uin = uin;
}

void SearchRecord::reqFirstName(const QString& firstName)
{
	this->firstName = firstName;
}

void SearchRecord::reqLastName(const QString& lastName) {
	this->lastName = lastName;
}

void SearchRecord::reqNickName(const QString& nickName)
{
	this->nickName = nickName;
}

void SearchRecord::reqCity(const QString& city)
{
	this->city = city;
}

void SearchRecord::reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo)
{
	this->birthYearFrom = birthYearFrom;
	this->birthYearTo = birthYearTo;
}

void SearchRecord::reqGender(bool female)
{
	this->gender = (female ? 2 : 1);
}

void SearchRecord::reqActive()
{
	this->active = true;
};

void SearchRecord::clearData()
{
	kdebugf();
	fromUin = 0;
	uin = "";
	firstName = "";
	lastName = "";
	nickName = "";
	city = "";
	birthYearFrom = "";
	birthYearTo = "";
	gender = 0;
	active = false;
}

/* SocketNotifiers */

SocketNotifiers::SocketNotifiers()
{
	kdebug("SocketNotifiers::SocketNotifiers()\n");

	snr = snw = NULL;
	h = NULL;
}

SocketNotifiers::~SocketNotifiers()
{
	kdebug("SocketNotifiers::~SocketNotifiers()\n");

	deleteSocketNotifiers();
}

void SocketNotifiers::setGGHttp(struct gg_http *nh)
{
	h = nh;
}

struct gg_http *SocketNotifiers::getGGHttp()
{
	return h;
}

void SocketNotifiers::createSocketNotifiers(struct gg_http *nh)
{
	kdebug("SocketNotifiers::createSocketNotifiers()\n");

	h = nh;
	
	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

}

void SocketNotifiers::deleteSocketNotifiers()
{
	kdebug("SocketNotifiers::deleteSocketNotifiers()\n");
	
	if (snr)
	{
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
	}

	if (snw)
	{
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
	}

}

void SocketNotifiers::recreateSocketNotifiers()
{
	kdebug("SocketNotifiers::recreateSocketNotifiers()\n");

	deleteSocketNotifiers();
	createSocketNotifiers(h);
	checkWrite();
}

void SocketNotifiers::checkWrite()
{
	kdebug("SocketNotifiers::checkWrite()\n");

	if (h->check & GG_CHECK_WRITE)
		snw->setEnabled(true);
}

void SocketNotifiers::dataReceived()
{
	kdebug("SocketNotifiers::dataReceived()\n");

	if (h->check & GG_CHECK_READ)
		emit socketEvent();
}

void SocketNotifiers::dataSent()
{
	kdebug("SocketNotifiers::dataSent()\n");

	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		emit socketEvent();
}

void GaduProtocol::initModule()
{
	gadu=new GaduProtocol();
	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ResendUserlist"), tr("Resend &userlist"), gadu, SLOT(sendUserList()),0,-1,2);
}

GaduProtocol::GaduProtocol() : QObject()
{
	registerSN = new SocketNotifiers();
	unregisterSN = new SocketNotifiers();
	remindSN = new SocketNotifiers();

	connect(registerSN, SIGNAL(socketEvent()), this, SLOT(registerSocketEvent()));
	connect(unregisterSN, SIGNAL(socketEvent()), this, SLOT(unregisterSocketEvent()));
	connect(remindSN, SIGNAL(socketEvent()), this, SLOT(remindSocketEvent()));

	connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
}

GaduProtocol::~GaduProtocol()
{
	delete registerSN;
	delete unregisterSN;

	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
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

	/* we were popping up sometimes, so let's keep the server informed */
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
	searchRecord.fromUin = 0;
	searchNextInPubdir(searchRecord);
}

void GaduProtocol::searchNextInPubdir(SearchRecord& searchRecord) {
	QString bufYear;
	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (searchRecord.uin.length())
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(searchRecord.uin).data());
	if (searchRecord.firstName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(searchRecord.firstName).data());
	if (searchRecord.lastName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(searchRecord.lastName).data());
	if (searchRecord.nickName.length())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(searchRecord.nickName).data());
	if (searchRecord.city.length())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(searchRecord.city).data());
	if (searchRecord.birthYearFrom.length() && searchRecord.birthYearTo.length()) {
		QString bufYear = searchRecord.birthYearFrom + " " + searchRecord.birthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufYear).data());
	}

	switch (searchRecord.gender) {
		case 1:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case 2:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
	}

	QString s = QString::number(searchRecord.fromUin);
	gg_pubdir50_add(req, GG_PUBDIR50_START, s.local8Bit());

	searchRecord.seq = gg_pubdir50(sess, req);
	gg_pubdir50_free(req);
}

void GaduProtocol::newResults(gg_pubdir50_t res) {
	int count, fromUin;
	SearchResult searchResult;
	SearchResults searchResults;

	count = gg_pubdir50_count(res);

	for (int i = 0; i < count; i++) {
		searchResult.setData(
			gg_pubdir50_get(res, i, GG_PUBDIR50_UIN),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR),
			gg_pubdir50_get(res, i, GG_PUBDIR50_CITY),
			gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS)
		);
		searchResults.append(searchResult);
	}
		
	fromUin = gg_pubdir50_next(res);

	emit newSearchResults(searchResults, res->seq, fromUin);
}

/* rejestrowanie u¿ytkownika */

bool GaduProtocol::doRegister(QString& mail, QString& password, QString& token_id, QString& token_value)
{
	// po co by³y te strdup'y ??
	struct gg_http *registerHttp = gg_register3(unicode2cp(mail).data(), unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (registerHttp)
	{
		registerSN->createSocketNotifiers(registerHttp);
		return true;
	}
	else
		return false;
}

void GaduProtocol::registerSocketEvent()
{
	kdebug("GaduProtocol::registerSocketEvent()\n");

	struct gg_http *registerHttp = registerSN->getGGHttp();

	if (gg_register_watch_fd(registerHttp) == -1)
	{
		registerSN->deleteSocketNotifiers();
		gg_free_register(registerHttp);
		kdebug("GaduProtocol::registerSocketEvent(): error registering\n");
		emit registered(false, 0);
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)registerHttp->data;

	switch (registerHttp->state)
	{
		case GG_STATE_CONNECTING:
			kdebug("GaduProtocol::registerSocketEvent(): changing QSocketNotifiers.\n");
			registerSN->recreateSocketNotifiers();
			break;

		case GG_STATE_ERROR:
			registerSN->deleteSocketNotifiers();
			gg_free_register(registerHttp);
			kdebug("GaduProtocol::registerSocketEvent(): error registering\n");
			emit registered (false, 0);
			break;

		case GG_STATE_DONE:
			registerSN->deleteSocketNotifiers();
			kdebug("GaduProtocol::registerSocketEvent(): success=%d, uin=%ld\n", p->success, p->uin);
			if (p->success)
				emit registered(true, p->uin);
			else
			{
				kdebug("GaduProtocol::registerSocketEvent(): error registering\n");
				emit registered(false, 0);
			}
			gg_free_register(registerHttp);
			break;

		default:
			registerSN->checkWrite();
	}
}

/* wyrejestrowywanie u¿ytkownika */

bool GaduProtocol::doUnregister(UinType uin, QString &password, QString& token_id, QString& token_value)
{
	// po co by³y te strdup'y ?
	struct gg_http* unregisterHttp = gg_unregister3(uin, unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (unregisterHttp)
	{
		unregisterSN->createSocketNotifiers(unregisterHttp);
		return true;
	}
	else
		return false;
}

void GaduProtocol::unregisterSocketEvent()
{
	kdebug("GaduProtocol::unregisterSocketEvent()\n");

	struct gg_http *unregisterHttp = unregisterSN->getGGHttp();

	if (gg_register_watch_fd(unregisterHttp) == -1 )
	{
		unregisterSN->deleteSocketNotifiers();
		gg_free_register(unregisterHttp);
		kdebug("GaduProtocol::unregisterSocketEvent(): error unregistering\n");
		emit unregistered(false);
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)unregisterHttp->data;

	switch (unregisterHttp->state)
	{
		case GG_STATE_CONNECTING:
			kdebug("GaduProtocol::unregisterSocketEvent(): changing QSocketNotifiers.\n");
			unregisterSN->recreateSocketNotifiers();
			break;

		case GG_STATE_ERROR:
			unregisterSN->deleteSocketNotifiers();
			gg_free_register(unregisterHttp);
			kdebug("GaduProtocol::unregisterSocketEvent(): error unregistering\n");
			emit unregistered(false);
			break;

		case GG_STATE_DONE:
			unregisterSN->deleteSocketNotifiers();
			kdebug("GaduProtocol::unregisterSocketEvent(): success\n");
			if (p->success)
				emit unregistered(true);
			else
			{
				kdebug("GaduProtocol::unregisterSocketEvent(): error unregistering\n");
				emit unregistered(false);
			}
			gg_free_register(unregisterHttp);
			break;
			
		default:
			unregisterSN->checkWrite();
	}
}

/* przypomnienie hasla */

bool GaduProtocol::doRemind(UinType uin, QString& token_id, QString& token_value)
{
	kdebug("GaduProtocol::remindPassword()\n");

	struct gg_http *remindHttp = gg_remind_passwd2(uin, unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (remindHttp)
	{
		remindSN->createSocketNotifiers(remindHttp);
		return true;
	}
	else
		return false;
}

void GaduProtocol::remindSocketEvent()
{
	kdebug("GaduProtocol::remindSocketEvent()\n");

	struct gg_http *remindHttp = remindSN->getGGHttp();

	if (gg_remind_passwd_watch_fd(remindHttp) == -1)
	{
		remindSN->deleteSocketNotifiers();
		emit reminded(false);
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)remindHttp->data;

	switch (remindHttp->state)
	{
		case GG_STATE_CONNECTING:
			kdebug("GaduProtocol::remindSocketEvent(): changing QSocketNotifiers.\n");
			remindSN->recreateSocketNotifiers();
			break;

		case GG_STATE_ERROR:
			kdebug("GaduProtocol::remindSocketEvent(): error reminding password!\n");
			remindSN->deleteSocketNotifiers();
			gg_free_remind_passwd(remindHttp);
			emit reminded(false);
			break;

		case GG_STATE_DONE:
			kdebug("GaduProtocol::remindSocketEvent(): success!\n");
			remindSN->deleteSocketNotifiers();

			if (p->success)
				emit reminded(true);
			{
				kdebug("GaduProtocol::remindSocketEvent(): error reminding password!\n");
				emit reminded(false);
			}
			gg_free_remind_passwd(remindHttp);
			break;

		default:
			remindSN->checkWrite();
	}

}

/* zmiana has³a */

bool GaduProtocol::doChangePassword(UinType uin, QString& mail, QString& password, QString& new_password, QString& token_id, QString& token_val)
{
	kdebug("GaduProtocol::doChangePassword()\n");

	struct gg_http *changePasswordHttp = gg_change_passwd4(uin, unicode2cp(mail).data(), unicode2cp(password).data(), 
			unicode2cp(new_password).data(), unicode2cp(token_id).data(), unicode2cp(token_val).data(), 1);
	if (changePasswordHttp)
	{
		changePasswordSN->createSocketNotifiers(changePasswordHttp);
		return true;
	}
	else
		return false;
}

void GaduProtocol::changePasswordSocketEvent()
{
	kdebug("GaduProtocol::changePasswordSocketEvent()\n");

	struct gg_http *changePasswordHttp = changePasswordSN->getGGHttp();

	if (gg_remind_passwd_watch_fd(changePasswordHttp) == -1)
	{
		changePasswordSN->deleteSocketNotifiers();
		emit passwordChanged(false);
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)changePasswordHttp->data;

	switch (changePasswordHttp->state)
	{
		case GG_STATE_CONNECTING:
			kdebug("GaduProtocol::changePasswordSocketEvent(): changing QSocketNotifiers\n");
			changePasswordSN->recreateSocketNotifiers();
			break;

		case GG_STATE_ERROR:
			kdebug("GaduProtocol::changePasswordSocketEvent(): error changing password!\n");
			changePasswordSN->deleteSocketNotifiers();
			gg_change_passwd_free(changePasswordHttp);
			emit passwordChanged(false);
			break;

		case GG_STATE_DONE:
			kdebug("GaduProtocol::changePasswordSocketEvent(): success!\n");
			changePasswordSN->deleteSocketNotifiers();

			if (p->success)
				emit passwordChanged(true);
			else
			{
				kdebug("GaduProtocol::changePasswordSocketEvent(): error changing password!\n");
				emit passwordChanged(false);
			}
			gg_free_change_passwd(changePasswordHttp);
			break;

		default:
			changePasswordSN->checkWrite();
			
	}
}

GaduProtocol* gadu;

