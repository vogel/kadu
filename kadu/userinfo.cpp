/***************************************************************************
                          userinfo.cpp  -  description
                             -------------------
    begin                : Tue Oct 9 2001
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

#include <klocale.h>
#include <sys/types.h>
#include <qpushbutton.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <qlayout.h>
#include <qmessagebox.h>

//
#include "kadu.h"
#include "userinfo.h"
//

UserInfo::UserInfo (const QString & name, QDialog* parent , const QString &altnick) : QDialog (parent, name) {
	resize(240,200);
	setCaption(i18n("User info on %1").arg(altnick));
	setWFlags(Qt::WDestructiveClose);

	e_uin = new QLineEdit(this);
	QLabel *l_uin = new QLabel(this);
	l_uin->setText(i18n("Uin"));

	QLineEdit *e_status = new QLineEdit(this);
	QLabel *l_status = new QLabel(this);
	l_status->setText(i18n("Status"));

	e_nickname = new QLineEdit(this);
	QLabel *l_nickname = new QLabel(this);
	l_nickname->setText(i18n("Nickname"));

	e_firstname = new QLineEdit(this);
	QLabel *l_firstname = new QLabel(this);
	l_firstname->setText(i18n("First name"));

	e_lastname = new QLineEdit(this);
	QLabel *l_lastname = new QLabel(this);
	l_lastname->setText(i18n("Surname"));

	e_altnick = new QLineEdit(this);
	QLabel *l_altnick = new QLabel(this);
	l_altnick->setText(i18n("AltNick"));

	e_mobile = new QLineEdit(this);
	QLabel *l_mobile = new QLabel(this);
	l_mobile->setText(i18n("Mobile"));

	e_group = new QLineEdit(this);
	QLabel *l_group = new QLabel(this);
	l_group->setText(i18n("Group"));

	e_addr = new QLineEdit(this);
	QLabel *l_addr = new QLabel(this);
	l_addr->setText(i18n("Address"));
	e_addr->setReadOnly(true);

//    e_altnick->setText(__c2q(UinToUser(uin)));

	QPushButton * update = new QPushButton(this);
	update->setText(i18n("Write userlist"));
	connect(update, SIGNAL( clicked() ), this, SLOT(writeUserlist()));

	int i;
	i = 0;
 	while (i < userlist.size() && userlist[i].altnick != altnick)
		i++;
	this_index = i;

	e_nickname->setText(userlist[i].nickname);
	e_altnick->setText(userlist[i].altnick);
	e_firstname->setText(userlist[i].first_name);
	e_lastname->setText(userlist[i].last_name);
 	e_mobile->setText(userlist[i].mobile);	
	e_group->setText(userlist[i].group);
	
	if (userlist[i].ip) {
		struct in_addr in;
		in.s_addr = userlist[i].ip;
		char address[128];
		snprintf(address, sizeof(address), "%s:%d", inet_ntoa(in), userlist[i].port);
		e_addr->setText(i18n(address));
		}
	else
		e_addr->setText(i18n("(Unknown)"));

	switch (userlist[i].status) {
		case GG_STATUS_AVAIL:
			e_status->setText(i18n("Online")); break;
		case GG_STATUS_AVAIL_DESCR:
			e_status->setText(i18n("Online (d.)")); break;
		case GG_STATUS_NOT_AVAIL:
			e_status->setText(i18n("Offline")); break;
		case GG_STATUS_NOT_AVAIL_DESCR:
			e_status->setText(i18n("Offline (d.)")); break;
		case GG_STATUS_BUSY:
			e_status->setText(i18n("Busy")); break;
		case GG_STATUS_BUSY_DESCR:
			e_status->setText(i18n("Busy (d.)")); break;
		case GG_STATUS_INVISIBLE2:
			e_status->setText(i18n("Invisible")); break;
		case GG_STATUS_BLOCKED:
			e_status->setText(i18n("Blocks us")); break;
		}

	if (!userlist_sent)
		e_status->setText(i18n("(Unknown)"));

	e_status->setReadOnly(true);
//	e_status->setEnabled(false);

	if (userlist[i].uin)
		e_uin->setText(QString::number(userlist[i].uin));
	e_uin->setReadOnly(true);
//	e_uin->setEnabled(false);

	QPushButton * kloze = new QPushButton(this);
	kloze->setText(i18n("Close"));
	connect(kloze, SIGNAL(clicked()), this, SLOT(close()));

	QGridLayout * grid = new QGridLayout(this, 10, 2, 10, 3);
	grid->addColSpacing(0,80);
	grid->addWidget(l_uin,0,0);
	grid->addWidget(e_uin,0,1);
	grid->addWidget(l_status,1,0);
	grid->addWidget(e_status,1,1);
	grid->addWidget(l_nickname,2,0);
	grid->addWidget(e_nickname,2,1);
	grid->addWidget(l_firstname,3,0);
	grid->addWidget(e_firstname,3,1);
	grid->addWidget(l_lastname,4,0);
	grid->addWidget(e_lastname,4,1);
	grid->addWidget(l_altnick,5,0);
	grid->addWidget(e_altnick,5,1);
	grid->addWidget(l_mobile,6,0);
	grid->addWidget(e_mobile,6,1);
	grid->addWidget(l_group,7,0);
	grid->addWidget(e_group,7,1);
	grid->addWidget(l_addr,8,0);
	grid->addWidget(e_addr,8,1);
	grid->addWidget(kloze,9,0);
	grid->addWidget(update,9,1);
}

void UserInfo::writeUserlist() {
	bool yes = false;
	switch (QMessageBox::information(kadu, "Kadu", i18n("This will write current userlist"), i18n("OK"), i18n("Cancel"), QString::null, 0, 1)) {
		case 0: // Yes?
			fprintf(stderr, "KK UserInfo::writeUserlist(): this_index: %d\n", this_index);
			userlist.changeUserInfo(userlist[this_index].altnick,
				e_firstname->text(),e_lastname->text(),
				e_nickname->text(),e_altnick->text(),
				e_mobile->text(),e_group->text());
			userlist.writeToFile();
			close();
			break;
		case 1:
			return;	
		}
}

#include "userinfo.moc"
