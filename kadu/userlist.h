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
#include "../libgadu/lib/libgadu.h"

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
	bool foreign;
	int ip;
	short port;
	int time_to_death;
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
		UserListElement& byNick(QString nickname);
		UserListElement& byAltNick(QString altnick);
		bool containsUin(uin_t uin);
		void addUser(const QString FirstName, const QString LastName,
			const QString NickName, const QString AltNick,
			const QString Mobile, const QString Uin,
			const int Status = GG_STATUS_NOT_AVAIL,
			const bool Blocking = false, const bool Offline_to_user = false,
			const bool Notify = true, const QString Group = "", const QString Description = "", const bool Foreign = false);
		void changeUserInfo(const QString OldAltNick,
			const QString &FirstName, const QString &LastName,
			const QString &NickName, const QString &AltNick,
			const QString &Mobile, const bool Blocking,
			const bool Offline_to_user, const bool Notify, const QString &Group);
		void changeUserStatus(const uin_t uin, const unsigned int status);
		void removeUser(const QString &altnick);
		bool writeToFile(char *filename = NULL);
		bool readFromFile();
		UserList &operator=(const UserList& userlist);
		
	protected:
		QTimer *invisibleTimer;

	private slots:
		void timeout();

	signals:
		void modified();
		void statusModified(UserListElement *);
};

#endif
