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
};

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
	append(e);
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
	if (AltNick != e.altnick)
	{
		UserBox::all_renameUser(e.altnick,AltNick);
		UserBox::all_refresh();			
	};	
	e.altnick=AltNick;
	e.mobile=Mobile;
	e.foreign=false;		
	e.group=Group;
//			if (!userlist[this_index].anonymous)
//				userlist[this_index].mobile = e_mobile->text();
	emit modified();
};

void UserList::removeUser(const QString &altnick)
{
	for (Iterator i = begin(); i != end(); i++)
		if((*i).altnick == altnick)
		{ 
			remove(i);
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

	// Trzeba dodac obsluge pl literek!!!!!!!!!!!!!!!
	
/*	    cp_to_iso((unsigned char *)first_name);
	    cp_to_iso((unsigned char *)userlist[i].last_name);
	    cp_to_iso((unsigned char *)userlist[i].nickname);
	    cp_to_iso((unsigned char *)userlist[i].comment);
	    cp_to_iso((unsigned char *)userlist[i].group);*/

		// if the nickname isn't defined explicitly, try to guess it 
	};

	f.close();
    	return true;
}

UserList& UserList::operator=(const UserList& userlist)
{
	QValueList<UserListElement>::operator=(userlist);
	return *this;
};

/*
int UserList::count()
{
	return List.count();
};

UserListElement& UserList::operator[](const int i)
{
	return List[i];
};*/

#include "userlist.moc"
