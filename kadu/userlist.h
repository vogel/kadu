/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef USERLIST_H
#define USERLIST_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qtimer.h>
#include <qhostaddress.h>
#include <qdns.h>
#include <qptrlist.h>

#include "libgadu.h"

class DnsHandler : public QObject
{
	Q_OBJECT

	public:
		DnsHandler(uin_t uin);
		~DnsHandler();
		bool isCompleted();

		static int counter;

	private:
		QDns dnsresolver;
		uin_t uin;
		bool completed;

	public slots:
		void resultsReady();
};

typedef QPtrList<DnsHandler> DnsLookups;

struct UserListElement
{
	QString first_name;
	QString last_name;
	QString nickname;
	QString altnick;
	QString mobile;
	QString group;
	QString description;
	uin_t uin;
	unsigned int status;
	bool anonymous;
	QHostAddress ip;
	QString dnsname;
	short port;
	bool blocking;
	bool offline_to_user;
	bool notify;
};

class UserList : public QObject, public QValueList<UserListElement>
{
	Q_OBJECT
	
	public:
		UserList();
		~UserList();
		UserListElement& byUin(uin_t uin);
		UserListElement& byNick(const QString& nickname);
		UserListElement& byAltNick(const QString& altnick);
		UserListElement byUinValue(uin_t uin);
		bool containsUin(uin_t uin);
		bool containsAltNick(const QString& altnick);
		void addUser(const QString& FirstName, const QString& LastName,
			const QString& NickName, const QString& AltNick,
			const QString& Mobile, const QString& Uin,
			const int Status = GG_STATUS_NOT_AVAIL,
			const bool Blocking = false, const bool Offline_to_user = false,
			const bool Notify = true, const QString& Group = "", const QString& Description = "", const bool anonymous = false);
		void changeUserInfo(const QString& OldAltNick,
			const QString &FirstName, const QString &LastName,
			const QString &NickName, const QString &AltNick,
			const QString &Mobile, const QString &Uin, int Status,
			const bool Blocking, const bool Offline_to_user, const bool Notify,
			const QString &Group);
		void changeUserStatus(const uin_t uin, const unsigned int status);
		void removeUser(const QString &altnick);
		bool writeToFile(QString filename = "");
		bool readFromFile();
		UserList &operator=(const UserList& userlist);
		void setDnsName(uin_t uin, const QString &name);
		void addDnsLookup(uin_t uin, const QHostAddress &ip);

	protected:
		DnsLookups dnslookups;

	signals:
		void modified();
		void statusModified(UserListElement *);
		void dnsNameReady(uin_t);
};

extern UserList userlist;

#endif
