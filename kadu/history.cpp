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
#include "misc.h"
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

	if (customtime) {
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

void appendHistory(UinsList uins, uin_t uin, unsigned char * msg, bool own, time_t time) {
	int i;
	
	QFile f;

	char * path2 = preparePath("history/");

	QString fname;
	
	fname = path2;
	
	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));
		if (i < uins.count() - 1)
			fname.append("_");
		}
		
	f.setName(fname);
	
	if (!(f.open(IO_WriteOnly | IO_Append))) {
		fprintf(stderr, "appendHistory(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}

	QString nick;
	
	if (!own) {
		if (userlist.containsUin(uin))
			nick = userlist.byUin(uin).altnick;
		else
			nick = QString::number(uin); 
		f.writeBlock(nick.local8Bit(), nick.length());
		}
	else
		f.writeBlock(config.nick.local8Bit(), config.nick.length());

	f.putch(' ');

	char *ctime;	
	if (!time)
		ctime = timestamp();
	else
		ctime = timestamp(time);
	f.writeBlock(ctime, strlen(ctime));

	f.putch('\n');
	
	QString mesg;
	mesg.append(__c2q((const char *)msg));
	mesg.append("\n\n");
	f.writeBlock(mesg.local8Bit(), mesg.length());

	f.close();
}

History::History(UinsList uins) {
	int i;
	
	setCaption(i18n("History"));
	setWFlags(Qt::WDestructiveClose);

	QGridLayout *grid = new QGridLayout(this, 2,1,3,3);

	body = new QMultiLineEdit(this, "History browser");
	body->setReadOnly(true);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setWrapPolicy(QMultiLineEdit::Anywhere);
	QPushButton *closebtn = new QPushButton(this);
	closebtn->setText(i18n("&Close"));

	QString fname;
	fname.append(getenv("HOME"));
	fname.append("/.gg/history/");
	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));	
		if (i < uins.count() - 1)
			fname.append("_");
		}
		
	QFile f(fname);
	if (f.open(IO_ReadOnly)) {
		QTextStream t(&f);
		body->setText(t.read());
		}
	else {
		fprintf(stderr, "KK History(): Error opening history file %s\n", (const char *)fname.local8Bit());
		body->setText(i18n("Error opening history file"));
		}
	grid->addWidget(body,0,0);
	grid->addWidget(closebtn,1,0, Qt::AlignRight);

	connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	resize(500,400);
}

#include "history.moc"
