/***************************************************************************
                          history.cpp  -  description
                             -------------------
    begin                : Sun Sep 9 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qwidget.h>
#include <qdialog.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qlayout.h>
#include <klocale.h>
#include <qfile.h>

//
#include "kadu.h"
#include "history.h"
//

char *timestamp(time_t customtime)
{
	static char buf[100];

	time_t t;
	struct tm *tm;

	time(&t);

	tm = localtime(&t);
	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T", tm);

	if (customtime != 0) {
		char buf2[20];
		struct tm *tm2;
		tm2 = localtime(&customtime);
		strftime(buf2, sizeof(buf2), " / S %T)", tm2);
    strncat(buf, buf2, sizeof(buf2));
		
/*		int j = 0;
		while(buf[j++] != "\0");
		
		int i = -1;
		while(buf2[++i] != "\0") {
       buf[j+i] = buf2[i];
			}
		buf[j + ++i] = "\0"; */
	
		return buf;

		}

	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T)", tm);

	return buf;
}

void appendHistory(unsigned int uin, unsigned char * msg, bool own, time_t time) {
	FILE *f;

/*	char * path = getenv("HOME");
	char * path2 = "/.gg/history/"; */

  char * path2 = preparePath("history/");

//  struct stat * boof;
//  cout << path2 << endl;

//  stat(path2, boof);
//	if (!S_ISDIR(boof->st_mode)) {
//			printf("KK appendHistory(): Creating directory\n");
//			if (mkdir(path2, 0700) != 0 ) {
//				perror("mkdir");
//				return;
//				}
//			}

	char buffer[512];
	snprintf(buffer,255,"%s%d",path2,uin);
//	char path3[255];
//	snprintf(path3,255,"%s%d",path2,uin);
//	char buffer[511];
//	snprintf(buffer,511,"%s%s",path,path3);

	if (!(f = fopen(buffer, "a"))) {
		fprintf(stderr, "appendHistory(): Error opening history file %s\n", buffer);
		return;
	}

	char nick[255];
	
	if (!own) {
		strncpy(nick, UinToUser(uin), 255);
		fputs(nick,f);
}
	else
		fputs("Me",f);

	fputs(" ", f);
	if (time == 0)
		fputs(timestamp(), f);
  else
		fputs(timestamp(time), f);

	fputs("\n", f);

	fputs((const char*)msg, f);
	fputs("\n\n",f);
		  	
  fclose(f);
}

History::History(uin_t uin) {
    setCaption(i18n("History"));
    setWFlags(Qt::WDestructiveClose);

    QGridLayout *grid = new QGridLayout(this, 2,1,3,3);

    body = new QMultiLineEdit(this, "History browser");
    body->setReadOnly(true);
    body->setWordWrap(QMultiLineEdit::WidgetWidth);
    body->setWrapPolicy(QMultiLineEdit::Anywhere);
    QPushButton *closebtn = new QPushButton(this);
    closebtn->setText(i18n("&Close"));

    char *path = getenv("HOME");
    char *path2 = "/.gg/history/";
    char path3[255];
    snprintf(path3,255,"%s%d",path2,uin);
    char buffer[511];
    snprintf(buffer,511,"%s%s",path,path3);

    QFile f(buffer);
    if (f.open(IO_ReadOnly)) {
        char *cbody = (char *) malloc(f.size()+1);
        f.readBlock(cbody, f.size());
        cbody[f.size()] = 0;
        body->setText(__c2q(cbody));
	}
    else {
        fprintf(stderr, "KK History(): Error opening history file %s\n", buffer);
        body->setText(i18n("Error opening history file"));
        }
    grid->addWidget(body,0,0);
    grid->addWidget(closebtn,1,0, Qt::AlignRight);

    connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

    resize(500,400);
}

#include "history.moc"
