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
#include <qvbox.h>

//
#include "kadu.h"
#include "userinfo.h"
//

#define	GG_USER_OFFLINE	0x01
#define	GG_USER_NORMAL	0x03
#define GG_USER_BLOCKED	0x04

UserInfo::UserInfo (const QString &name, QDialog *parent , const QString &altnick)
: QTabDialog(parent, name) {
	resize(200,200);
	setCaption(i18n("User info on %1").arg(altnick));
	setWFlags(Qt::WDestructiveClose);

	int i = 0;
 	while (i < userlist.size() && userlist[i].altnick != altnick)
		i++;
	this_index = i;

	setupTab1();
	setupTab2();

	setOkButton(i18n("Write userlist"));
	setCancelButton(i18n("Close"));

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(writeUserlist()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));

}

void UserInfo::setupTab1() {
	QVBox *box = new QVBox(this);
	box->setMargin(10);

//	QHBox *hbox1 = new QHBox(box);
	QLabel *l_uin = new QLabel(box);
	l_uin->setText(i18n("Uin"));
	e_uin = new QLineEdit(box);

//	QHBox *hbox2 = new QHBox(box);
	QLabel *l_status = new QLabel(box);
	l_status->setText(i18n("Status"));
	QLineEdit *e_status = new QLineEdit(box);

//	QHBox *hbox3 = new QHBox(box);
	QLabel *l_nickname = new QLabel(box);
	l_nickname->setText(i18n("Nickname"));
	e_nickname = new QLineEdit(box);

//	QHBox *hbox4 = new QHBox(box);
	QLabel *l_firstname = new QLabel(box);
	l_firstname->setText(i18n("First name"));
	e_firstname = new QLineEdit(box);

//	QHBox *hbox5 = new QHBox(box);
	QLabel *l_lastname = new QLabel(box);
	l_lastname->setText(i18n("Surname"));
	e_lastname = new QLineEdit(box);

//	QHBox *hbox6 = new QHBox(box);
	QLabel *l_altnick = new QLabel(box);
	l_altnick->setText(i18n("AltNick"));
	e_altnick = new QLineEdit(box);

//	QHBox *hbox7 = new QHBox(box);
	QLabel *l_mobile = new QLabel(box);
	l_mobile->setText(i18n("Mobile"));
	e_mobile = new QLineEdit(box);

//	QHBox *hbox8 = new QHBox(box);
	QLabel *l_group = new QLabel(box);
	l_group->setText(i18n("Group"));
	e_group = new QLineEdit(box);

//	QHBox *hbox9 = new QHBox(box);
	QLabel *l_addr = new QLabel(box);
	l_addr->setText(i18n("Address"));
	e_addr = new QLineEdit(box);
	e_addr->setReadOnly(true);

	e_nickname->setText(userlist[this_index].nickname);
	e_altnick->setText(userlist[this_index].altnick);
	e_firstname->setText(userlist[this_index].first_name);
	e_lastname->setText(userlist[this_index].last_name);
 	e_mobile->setText(userlist[this_index].mobile);	
	e_group->setText(userlist[this_index].group);
	
	if (userlist[this_index].ip) {
		struct in_addr in;
		in.s_addr = userlist[this_index].ip;
		char address[128];
		snprintf(address, sizeof(address), "%s:%d", inet_ntoa(in), userlist[this_index].port);
		e_addr->setText(i18n(address));
		}
	else
		e_addr->setText(i18n("(Unknown)"));

	switch (userlist[this_index].status) {
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

	if (userlist[this_index].uin)
		e_uin->setText(QString::number(userlist[this_index].uin));
	e_uin->setReadOnly(true);

/*	QGridLayout * grid = new QGridLayout(box, 10, 2, 10, 3);
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
	grid->addWidget(e_addr,8,1);*/

	addTab(box, i18n("General"));
}

void UserInfo::setupTab2() {
	QVBox *box = new QVBox(this);
	box->setMargin(10);

	c_blocking = new QCheckBox(i18n("Block user"), box);
	c_blocking->setChecked(userlist[this_index].blocking);

	c_offtouser = new QCheckBox(i18n("Offline to user"), box);
	c_offtouser->setChecked(userlist[this_index].offline_to_user);

	c_notify = new QCheckBox(i18n("Notify about status changes"), box);
	c_notify->setChecked(userlist[this_index].notify);

	addTab(box, i18n("Others"));
}

void UserInfo::writeUserlist() {
	bool yes = false;
	switch (QMessageBox::information(kadu, "Kadu", i18n("This will write current userlist"), i18n("OK"), i18n("Cancel"), QString::null, 0, 1)) {
		case 0: // Yes?
			fprintf(stderr, "KK UserInfo::writeUserlist(): this_index: %d\n", this_index);
			if (sess && sess->status != GG_STATUS_NOT_AVAIL) {
				if (c_offtouser->isChecked() && !userlist[this_index].offline_to_user) {
					gg_remove_notify_ex(sess, userlist[this_index].uin, GG_USER_NORMAL);
					gg_add_notify_ex(sess, userlist[this_index].uin, GG_USER_OFFLINE);
					}
				else
					if (!c_offtouser->isChecked() && userlist[this_index].offline_to_user) {
						gg_remove_notify_ex(sess, userlist[this_index].uin, GG_USER_OFFLINE);
						gg_add_notify_ex(sess, userlist[this_index].uin, GG_USER_NORMAL);
						}
					else
						if (c_blocking->isChecked() && !userlist[this_index].blocking) {
							gg_remove_notify_ex(sess, userlist[this_index].uin, GG_USER_NORMAL);
							gg_add_notify_ex(sess, userlist[this_index].uin, GG_USER_BLOCKED);
							}
						else
							if (!c_blocking->isChecked() && userlist[this_index].blocking) {
								gg_remove_notify_ex(sess, userlist[this_index].uin, GG_USER_BLOCKED);
								gg_add_notify_ex(sess, userlist[this_index].uin, GG_USER_NORMAL);
								}
				}
			userlist.changeUserInfo(userlist[this_index].altnick,
				e_firstname->text(),e_lastname->text(),
				e_nickname->text(),e_altnick->text(),
				e_mobile->text(), c_blocking->isChecked(),
				c_offtouser->isChecked(), c_notify->isChecked(), e_group->text());
			userlist.writeToFile();
			close();
			break;
		case 1:
			return;	
		}
}

#include "userinfo.moc"
