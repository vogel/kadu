/***************************************************************************
                        userlist.cpp  -  description
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

#include <qfile.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "userlist.h"
#include "misc.h"
#include "kadu.h"
#include "userbox.h"

UserList::UserList() : QObject(), QValueList<UserListElement>()
{
	invisibleTimer = new QTimer;
	connect(invisibleTimer, SIGNAL(timeout()), this, SLOT(timeout()));
	invisibleTimer->start(1000, TRUE);
};

UserList::~UserList()
{
	invisibleTimer->stop();
	delete invisibleTimer;
}

void UserList::timeout()
{
	for (iterator i = begin(); i != end(); i++) {
		UserListElement &ule = *i;
		if (ule.status == GG_STATUS_INVISIBLE || ule.status == GG_STATUS_INVISIBLE_DESCR ||
			ule.status == GG_STATUS_INVISIBLE2) {
			ule.time_to_death--;
			if (!ule.time_to_death)
				changeUserStatus(ule.uin, GG_STATUS_NOT_AVAIL);
			else
				if (ule.time_to_death < 0)
					ule.time_to_death = 0;
			}
		}
	invisibleTimer->start(1000, TRUE);
}

UserListElement& UserList::byUin(uin_t uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return (*i);
	fprintf(stderr, "KK UserList::byUin(): Panic!\n");
	// Kadu Panic :) What we should do here???
};

UserListElement& UserList::byNick(QString nickname)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).nickname==nickname)
			return (*i);
	fprintf(stderr, "KK UserList::byNick(): Panic! %s not exists\n",(const char*)nickname.local8Bit());
	// Kadu Panic :) What we should do here???
};

UserListElement& UserList::byAltNick(QString altnick)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).altnick==altnick)
			return (*i);
	fprintf(stderr, "KK UserList::byAltNick(): Panic! %s not exists\n",(const char*)altnick.local8Bit());
	// Kadu Panic :) What we should do here???
};

bool UserList::containsUin(uin_t uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return true;
	fprintf(stderr, "KK UserList::containsUin(): userlist doesnt contain %d\n", uin);
	return false;
};

void UserList::addUser(const QString FirstName,const QString LastName,
	const QString NickName,const QString AltNick,
	const QString Mobile,const QString Uin,const int Status,
	const QString Group,const QString Description, const bool Foreign)
{
	UserListElement e;
	e.first_name = FirstName;
	e.last_name = LastName;
	e.nickname = NickName;
	e.altnick = AltNick;
	e.mobile = Mobile;
	e.uin = atoi(Uin.local8Bit());
	e.status = Status;
	e.group = Group;
	e.description = Description;
	e.anonymous = false;
	e.foreign = Foreign;
	e.ip = 0;
	e.port = 0;
	e.time_to_death = 300;
	append(e);
	emit modified();
};

void UserList::changeUserInfo(const QString OldAltNick,
	const QString& FirstName, const QString& LastName,
	const QString& NickName, const QString& AltNick,
	const QString& Mobile,const QString& Group)
{
	UserListElement& e=byAltNick(OldAltNick);
	e.first_name = FirstName;
	e.last_name = LastName;
	e.nickname = NickName;
	e.altnick=AltNick;
	e.mobile=Mobile;
	e.foreign=false;		
	e.group=Group;
	if (AltNick != OldAltNick)
	{
		UserBox::all_renameUser(OldAltNick,AltNick);
		UserBox::all_refresh();			
	};	
	emit modified();
};

void UserList::changeUserStatus(const uin_t uin, const unsigned int status)
{
	UserListElement &e = byUin(uin);
	if (status != e.status) {
		e.status = status;
		UserBox::all_refresh();			
		emit statusModified(&e);
		}
};

void UserList::removeUser(const QString &altnick)
{
	for (Iterator i = begin(); i != end(); i++)
		if((*i).altnick == altnick)
		{ 
			remove(i);
			emit modified();
			break;
		};
};

bool UserList::writeToFile(char *filename)
{
	char *tmp;

	if (!(tmp = preparePath("")))
		return FALSE;
	mkdir(tmp, 0700);

	if (!filename)
	{
		if (!(filename = preparePath("userlist")))
			FALSE;
	};

	QFile f(filename);

	if (!f.open(IO_WriteOnly))
	{
		fprintf(stderr,"KK UserList::writeToFile(): Error opening file :(\n");
		return false;
	};

//	fchmod(fileno(f), 0600);

	for (Iterator i=begin(); i!=end(); i++)
	{
		QString s="";
		s.append((*i).first_name);
		s.append(QString(";"));
		s.append((*i).last_name);
		s.append(QString(";"));
		s.append((*i).nickname);
		s.append(QString(";"));
		s.append((*i).altnick);
		s.append(QString(";"));
		s.append((*i).mobile);
		s.append(QString(";"));
		s.append((*i).group);
		s.append(QString(";"));
		s.append(QString::number((*i).uin));
		s.append(QString("\r\n"));
		if (!(*i).foreign) {
			fprintf(stderr,s.local8Bit());
			f.writeBlock(s.local8Bit(),s.length());
			}
	}	    
	f.close();
	return true;
}

bool UserList::readFromFile()
{
	char * path = preparePath("userlist");
	fprintf(stderr, "KK UserList::readFromFile(): Opening userlist file: %s\n",path);

	QFile f(path);
	if(!f.open(IO_ReadOnly))
	{
		fprintf(stderr, "KK UserList::readFromFile(): Error opening userlist file");
		return false;
	}

	fprintf(stderr, "KK UserList::readFromFile(): File opened successfuly\n");
	    
	clear();

	QTextStream t(&f);
	QString line;
	while ((line = t.readLine()).length())
	{
		if (line[0] == '#')
			continue;

		if (line.find(';')<0)
		{
			QString nickname=line.section(' ',0,0);
			QString uin=line.section(' ',1,1);
			if(uin=="")
				continue;
			addUser("","",nickname,nickname,
				"",uin,GG_STATUS_NOT_AVAIL,"","");
		}
		else
		{	    
			QString first_name = line.section(';',0,0);
			QString last_name = line.section(';',1,1);
			QString nickname = line.section(';',2,2);
			QString altnick = line.section(';',3,3);
			QString mobile = line.section(';',4,4);
			QString group = line.section(';',5,5);
			QString uin = line.section(';',6,6);

			if(uin=="")
				continue;
				
			if(altnick=="")
			{
				if(nickname=="")
					altnick=first_name;
				else
					altnick=nickname;
			};

			addUser(first_name,last_name,nickname,altnick,
				mobile,uin,GG_STATUS_NOT_AVAIL,group,"");

		};

	};

	f.close();
	emit modified();
    	return true;
}

UserList& UserList::operator=(const UserList& userlist)
{
	QValueList<UserListElement>::operator=(userlist);
	emit modified();
	return *this;
};

#include "userlist.moc"
