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

SocketNotifiers::SocketNotifiers(struct gg_http* nh)
{
	kdebug("SocketNotifiers::SocketNotifiers()\n");

	h = nh;
}

SocketNotifiers::~SocketNotifiers()
{
	kdebug("SocketNotifiers::~SocketNotifiers()\n");

	deleteSocketNotifiers();
}

void SocketNotifiers::start()
{
	createSocketNotifiers();
}

void SocketNotifiers::createSocketNotifiers()
{
	kdebug("SocketNotifiers::createSocketNotifiers()\n");

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
	createSocketNotifiers();

	if (h->check & GG_CHECK_WRITE)
		snw->setEnabled(true);

}

void SocketNotifiers::dataReceived()
{
	kdebug("SocketNotifiers::dataReceived()\n");

	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void SocketNotifiers::dataSent()
{
	kdebug("SocketNotifiers::dataSent()\n");

	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void SocketNotifiers::socketEvent()
{
	kdebug("SocketNotifiers::socketEvent()\n");

	if (gg_pubdir_watch_fd(h) == -1)
	{
		deleteSocketNotifiers();
		emit done(false, h);
		gg_pubdir_free(h);
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)h->data;

	switch (h->state)
	{
		case GG_STATE_CONNECTING:
			kdebug("SocketNotifiers::socketEvent(): changing QSocketNotifiers\n");
			recreateSocketNotifiers();
			break;

		case GG_STATE_ERROR:
			kdebug("SocketNotifiers::socketEvent(): error!\n");
			deleteSocketNotifiers();
			emit done(false, h);
			gg_pubdir_free(h);
			deleteLater();
			return;

		case GG_STATE_DONE:
			kdebug("SocketNotifiers::socketEvent(): success!\n");
			deleteSocketNotifiers();

			if (p->success)
				emit done(true, h);
			else
			{
				kdebug("SocketNotifiers::socketEvent(): error!\n");
				emit done(false, h);
			}
			gg_pubdir_free(h);
			break;

		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
	}
}

/* GaduProtocol */

void GaduProtocol::initModule()
{
	gadu=new GaduProtocol();
//	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ResendUserlist"), tr("Resend &userlist"), gadu, SLOT(sendUserList()),0,-1,2);
}

GaduProtocol::GaduProtocol() : QObject()
{
	connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
}

GaduProtocol::~GaduProtocol()
{
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
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
	struct gg_http *h = gg_register3(unicode2cp(mail).data(), unicode2cp(password).data(), unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		SocketNotifiers *sn = new SocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(registerDone(bool, struct gg_http *)));
		sn->start();
		return true;
	}
	else
	{
		emit registered(false, 0);
		return false;
	}
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
		SocketNotifiers *sn = new SocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(unregisterDone(bool, struct gg_http *)));
		sn->start();
		return true;
	}
	else
	{
		emit unregistered(false);
		return false;
	}
}

void GaduProtocol::unregisterDone(bool ok, struct gg_http *)
{
	emit unregistered(ok);
}

/* przypomnienie hasla */

bool GaduProtocol::doRemind(UinType uin, QString& token_id, QString& token_value)
{
	kdebug("GaduProtocol::remindPassword()\n");

	struct gg_http *h = gg_remind_passwd2(uin, unicode2cp(token_id).data(), unicode2cp(token_value).data(), 1);
	if (h)
	{
		SocketNotifiers *sn = new SocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(remindDone(bool, struct gg_http *)));
		sn->start();
		return true;
	}
	else
	{
		emit reminded(false);
		return false;
	}
}

void GaduProtocol::remindDone(bool ok, struct gg_http *)
{
	emit reminded(ok);
}

/* zmiana has³a */

bool GaduProtocol::doChangePassword(UinType uin, QString& mail, QString& password, QString& new_password, QString& token_id, QString& token_val)
{
	kdebug("GaduProtocol::doChangePassword()\n");

	struct gg_http *h = gg_change_passwd4(uin, unicode2cp(mail).data(), unicode2cp(password).data(), 
			unicode2cp(new_password).data(), unicode2cp(token_id).data(), unicode2cp(token_val).data(), 1);
	if (h)
	{
		SocketNotifiers *sn = new SocketNotifiers(h);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(changePasswordDone(bool, struct gg_http *)));
		sn->start();
		return true;
	}
	else
	{
		emit passwordChanged(false);
		return false;
	}
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
	kdebug("GaduProtocol::userlistReplyReceived()\n");

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

