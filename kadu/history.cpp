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
#include <qfile.h>
#include <time.h>

//
#include "kadu.h"
#include "misc.h"
#include "history.h"
//

void appendHistory(UinsList uins, uin_t uin,unsigned char* msg, bool own, time_t time) {
	int i;
	
	QFile f;

	QString path2 = ggPath("history/");

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

void appendSMSHistory(const QString& mobile,const QString& msg)
{
	fprintf(stderr,"Appending sms to history (%s)\n",mobile.local8Bit().data());
	QString altnick;
	// Jesli user posiada uin to loguj to normalnej historii
	for(int i=0; i<userlist.count(); i++)
		if(userlist[i].mobile==mobile)
		{
			altnick=userlist[i].altnick;
			uin_t uin=userlist[i].uin;
			if(uin!=0)
			{
				UinsList uins;
				uins.append(userlist[i].uin);
				QCString cmsg=(QString("[SMS: ")+mobile+"]\n"+msg).local8Bit();
				unsigned char* umsg=(unsigned char*)cmsg.data();
				appendHistory(uins,userlist[i].uin,umsg,true);
			};
			break;
		};
	// Oprocz tego zawsze do pliku history/sms
	QFile f(ggPath("history/sms"));
	if(!(f.open(IO_WriteOnly|IO_Append)))
	{
		fprintf(stderr, "appendSMSHistory(): Error opening sms history file\n");
		return;
	};
	QString target;
	if(altnick.length()>0)
		target=altnick+" ("+mobile+")";
	else
		target=mobile;
	f.writeBlock(target.local8Bit(),target.length());
	f.putch(' ');
	char *ctime=timestamp();
	f.writeBlock(ctime, strlen(ctime));
	f.putch('\n');
	QString log=msg+"\n\n";
	f.writeBlock(log.local8Bit(),log.length());
	f.close();
};

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
	fname.append(ggPath("history/"));
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

