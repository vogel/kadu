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

UserList::UserList() : QValueList<UserListElement>()
{
};

UserListElement& UserList::byUin(uin_t uin)
{
	for(int i=0; i<size(); i++)
		if(operator[](i).uin==uin)
			return operator[](i);
	fprintf(stderr, "KK UserList::byUin(): Panic!\n");
	// Kadu Panic :) What we should do here???
//	return NULL;
};

UserListElement& UserList::byComment(QString comment)
{
	for(int i=0; i<userlist.size(); i++)
		if(operator[](i).comment==comment)
			return operator[](i);
	fprintf(stderr, "KK UserList::byComment(): Panic!\n");
	// Kadu Panic :) What we should do here???
//	return;
};

void UserList::addUser(const QString FirstName,const QString LastName,
	const QString NickName,const QString AltNick,
	const QString Mobile,const QString Uin,const int Status,
	const QString Group,const QString Description)
{
	UserListElement e;
	e.first_name = FirstName;
	e.last_name = LastName;
	e.nickname = NickName;
	e.comment = AltNick;
	e.mobile = Mobile;
	e.uin = atoi(Uin.local8Bit());
	e.status = Status;
	e.group = Group;
	e.description = Description;
	e.anonymous = false;
	e.ip = 0;
	e.port = 0;
	append(e);
};

void UserList::removeUser(uin_t uin)
{
	for(Iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
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
		return FALSE;
	};

//	fchmod(fileno(f), 0600);

	for (Iterator i=begin(); i!=end(); i++)
	{
		QString s="";
		s.append((*i).first_name);
		s.append(QString(";"));
		s.append((*i).last_name);
		s.append(QString(";"));
		s.append((*i).comment);
		s.append(QString(";"));
		s.append((*i).nickname);
		s.append(QString(";"));
		s.append((*i).mobile);
		s.append(QString(";"));
		s.append((*i).group);
		s.append(QString(";"));
		s.append(QString::number((*i).uin));
		s.append(QString("\r\n"));
		fprintf(stderr,s.local8Bit());
		f.writeBlock(s.local8Bit(),s.length());
	}	    
	f.close();
	return TRUE;
}

bool UserList::readFromFile()
{
	char * path = preparePath("userlist");
	printf("KK UserList::readFromFile(): Opening userlist file: %s\n",path);

	QFile f(path);
	if(f.open(IO_ReadOnly))
	{
		fprintf(stderr, "KK UserList::readFromFile(): Error opening userlist file");
		return FALSE;
	}

	printf("KK UserList::readFromFile(): File opened successfuly\n");
	    
	clear();

	QString line;
	while (f.readLine(line,1000))
	{
		if (line[0] == '#')
			continue;

		if (line.find(';')<0)
		{
			QString comment=comment.section(' ',0,0);
			QString uin=comment.section(' ',1,1);
			if(uin=="")
				continue;
			addUser("","","",comment,
				"",uin,GG_STATUS_NOT_AVAIL,"","");
		}
		else
		{	    
			QString first_name = line.section(';',0,0);
			QString last_name = line.section(';',1,1);
			QString comment = line.section(';',2,2);
			QString nickname = line.section(';',3,3);
			QString mobile = line.section(';',4,4);
			QString group = line.section(';',5,5);
			QString uin = line.section(';',6,6);

      /* load groups */
/*
      if (group != NULL && QString::compare(group, "") != 0) {
        bool already = false;
        for (int f = 0; f < grouplist.size(); f++) {
          if (QString::compare(__c2q(grouplist[f].name), __c2q(group)) == 0) {
            already = true;
            break;
            }
          }
        if (!already) {
          grouplist.resize(grouplist.size()+1);
          if (grouplist.size() > 1)
            grouplist[grouplist.size()-1].number = grouplist[grouplist.size()-2].number + 1;
          else
            grouplist[grouplist.size()-1].number = 601;
          grouplist[grouplist.size()-1].name = strdup(group);
        }
      } */

			if(uin=="")
				continue;
				
			if(nickname=="")
			{
				if(comment=="")
					nickname=first_name;
				else
					nickname=comment;
			};

			addUser(first_name,last_name,nickname,comment,
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
    	return TRUE;
}

//#include "userlist.moc"
