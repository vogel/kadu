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
SearchResult::SearchResult() {
}

SearchResult::SearchResult(const SearchResult& copyFrom) {
	this->uin = copyFrom.uin;
	this->first = copyFrom.first;
	this->nick = copyFrom.nick;
	this->born = copyFrom.born;
	this->city = copyFrom.city;
	this->status = copyFrom.status;
}

void SearchResult::setData(const char *uin, const char *first, const char *nick, const char *born, const char *city, const char *status) {
	this->uin = cp2unicode((unsigned char *)uin);
	this->first = cp2unicode((unsigned char *)first);
	this->nick = cp2unicode((unsigned char *)nick);
	this->born = cp2unicode((unsigned char *)born);
	this->city = cp2unicode((unsigned char *)city);
	this->status = atoi(status) & 127;
}

SearchRecord::SearchRecord() {
	kdebugf();
	clearData();
	kdebugf2();
}

SearchRecord::~SearchRecord() {
}

void SearchRecord::reqUin(const QString& uin) {
	this->uin = uin;
}

void SearchRecord::reqFirstName(const QString& firstName) {
	this->firstName = firstName;
}

void SearchRecord::reqLastName(const QString& lastName) {
	this->lastName = lastName;
}

void SearchRecord::reqNickName(const QString& nickName) {
	this->nickName = nickName;
}

void SearchRecord::reqCity(const QString& city) {
	this->city = city;
}

void SearchRecord::reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo) {
	this->birthYearFrom = birthYearFrom;
	this->birthYearTo = birthYearTo;
}

void SearchRecord::reqGender(bool female) {
	this->gender = (female ? 2 : 1);
}

void SearchRecord::reqActive() {
	this->active = true;
};

void SearchRecord::clearData() {
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

void GaduProtocol::initModule()
{
	gadu=new GaduProtocol();
	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ResendUserlist"), tr("Resend &userlist"), gadu, SLOT(sendUserList()),0,-1,2);
}

GaduProtocol::GaduProtocol() : QObject()
{
	connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
}

GaduProtocol::~GaduProtocol()
{
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(newResults(gg_pubdir50_t)));
}

int GaduProtocol::sendMessage(const UinsList& uins,const char* msg)
{
	kdebugf();
	int seq;
	if(uins.count()>1)
	{
		uin_t* users = new uin_t[uins.count()];
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
		uin_t* users = new uin_t[uins.count()];
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
	uin_t *uins;
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

	uins = (uin_t *) malloc(j * sizeof(uin_t));
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

bool GaduProtocol::sendImageRequest(uin_t uin,int size,uint32_t crc32)
{
	kdebugf();
	int res = gg_image_request(sess, uin, size, crc32);
	kdebugf2();
	return (res==0);
}

bool GaduProtocol::sendImage(uin_t uin,const QString& file_name,uint32_t size,char* data)
{
	kdebugf();
	int res = gg_image_reply(sess, uin, file_name.local8Bit().data(), data, size);
	kdebugf2();
	return (res==0);
}


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
	int count, statusCode, fromUin;
	const char *uin, *first, *nick, *born, *city, *status;
	SearchResult searchResult;
	SearchResults searchResults;

	count = gg_pubdir50_count(res);

	if (count < 1)
		return;

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

GaduProtocol* gadu;

