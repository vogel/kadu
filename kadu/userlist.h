/***************************************************************************
                         userlist.h  -  description
                             -------------------
    begin                : 15.07.2002
    copyright            : (C) 2002 by A.Smarzewski
    email                : adrians@aska.com.pl
 ***************************************************************************/

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
#include "../libgadu/lib/libgadu.h"

struct UserListElement
{
	QString first_name;
	QString last_name;
	QString nickname;
	QString comment;
	QString mobile;
	QString group;
	QString description;
	uin_t uin;
	unsigned int status;
	bool anonymous;
	int ip;
	short port;
};

class UserList : public QValueList<UserListElement>
{
//	Q_OBJECT
	
	public:
		UserList();
		UserListElement& byUin(uin_t uin);
		UserListElement& byComment(QString comment);
		bool containsUin(uin_t uin);
		void addUser(const QString FirstName,const QString LastName,
			const QString NickName,const QString AltNick,
			const QString Mobile,const QString Uin,
			const int Status=GG_STATUS_NOT_AVAIL,
			const QString Group="",const QString Description="");
		void removeUser(uin_t uin);
		bool writeToFile(char *filename = NULL);
		bool readFromFile();
};

#endif
