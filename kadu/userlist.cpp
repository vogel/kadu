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

#include "userlist.h"
#include "kadu.h"
//#include <stdio.h>
#include <qfile.h>
//#include <iostream.h>
//#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

static char *get_token(char **ptr, char sep)
{
	char *foo, *res;

	if (!ptr || !sep || !*ptr)
		return NULL;

	res = *ptr;

	if (!(foo = strchr(*ptr, sep)))
		*ptr += strlen(*ptr);
	else {
		*ptr = foo + 1;
		*foo = 0;
	}

	return res;
}

static char *read_file(FILE *f) {
	char buf[1024], *nju, *res = NULL;

	while (fgets(buf, sizeof(buf) - 1, f)) {
		int new_size = ((res) ? strlen(res) : 0) + strlen(buf) + 1;

		if (!((void *)nju = realloc(res, new_size))) {
			/* jesli brakuje pamieci, pomijamy reszte linii */
			if (strchr(buf, '\n'))
				break;
			else
				continue;
		}
		if (!res)
			*nju = 0;
		res = nju;
		strcpy(res + strlen(res), buf);

		if (strchr(buf, '\n'))
			break;
	}

	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\n')
		res[strlen(res) - 1] = 0;
	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\r')
		res[strlen(res) - 1] = 0;


	return res;
}

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

int UserList::writeToFile(char *filename)
{
	char *tmp;

	if (!(tmp = preparePath("")))
		return -1;
	mkdir(tmp, 0700);

	if (!filename)
	{
		if (!(filename = preparePath("userlist")))
			return -1;
	};

	QFile f(filename);

	if (!f.open(IO_WriteOnly))
	{
		fprintf(stderr,"KK UserList::writeToFile(): Error opening file :(\n");
		return -2;
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
	return 0;
}

int UserList::readFromFile()
{
/*	clear();

    char * path;
    struct passwd *pw;

    if (!(pw = getpwuid(getuid())))
	path = getenv("HOME");
    path = pw->pw_dir;

    char * path2 = "/.gg/userlist";
    char buffer[255];
    snprintf(buffer,255,"%s%s",path,path2);

    printf("KK readUserlist(): Opening userlist file: %s\n", buffer);

    FILE *f;
    char * buf;
    int i = 0;
    
    if (!(f = fopen(buffer, "r"))) {
	fprintf(stderr, "KK readUserlist(): Error opening userlist file");
	return -1;
	}

    userlist_count = 0;

    printf("KK readUserlist(): File opened successfuly\n");

    while ((buf = read_file(f))) {
	char *comment;

	if (buf[0] == '#') {
	    free(buf);
	    continue;
	    }

	if (!strchr(buf, ';')) {
	    if (!(comment = strchr(buf, ' '))) {
		free(buf);
		continue;
		}
	    
	    uin_t uin;
	    uin = strtol(buf, NULL, 0);

	    if (!uin) {
		free(buf);
		continue;
		}

	void UserList::addUser("","","",const QString& AltNick,++comment,
		"",uin,GG_STATUS_NOT_AVAIL,"","")
	    }
	else {
	    char *first_name, *last_name, *nickname, *comment, *mobile, *group, *uin,
	    *foo = buf;
	    uin_t uint;
	    
	    first_name = get_token(&foo, ';');
	    last_name = get_token(&foo, ';');
	    comment = get_token(&foo, ';');
	    nickname = get_token(&foo, ';');
	    mobile = get_token(&foo, ';');
	    group = get_token(&foo, ';');
	    uin = get_token(&foo, ';');*/

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

	    /*
	    if (!uin || !(uint = strtol(uin, NULL, 0))) {
		free(buf);
		continue;
		}

	    cp_to_iso((unsigned char *)first_name);
	    cp_to_iso((unsigned char *)userlist[i].last_name);
	    cp_to_iso((unsigned char *)userlist[i].nickname);
	    cp_to_iso((unsigned char *)userlist[i].comment);
	    cp_to_iso((unsigned char *)userlist[i].group);

	    // if the nickname isn't defined explicitly, try to guess it 
	    if (!QString::compare(nickname, ""))
		if (!QString::compare(comment, ""))
		    strcpy(nickname, first_name);
		else
		    strcpy(nickname, comment);
	    }

	addUser(first_name,last_name,nickname,comment
	mobile,uint,GG_STATUS_NOT_AVAIL,group,"")

	free(buf);
	
	i++;
	}

    fclose(f);*/
    return 0;
}

//#include "userlist.moc"
