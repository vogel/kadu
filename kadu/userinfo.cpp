/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
#include "chat.h"
//

UserInfo::UserInfo (const QString &name, QDialog *parent , const QString &altnick)
: QTabDialog(parent, name) {
	resize(200,200);
	setCaption(i18n("User info on %1").arg(altnick));
	setWFlags(Qt::WDestructiveClose);

	int i = 0;
 	while (i < userlist.size() && userlist[i].altnick != altnick)
		i++;
	puser = &userlist[i];

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

	QLabel *l_uin = new QLabel(box);
	l_uin->setText(i18n("Uin"));
	e_uin = new QLineEdit(box);

	QLabel *l_status = new QLabel(box);
	l_status->setText(i18n("Status"));
	QLineEdit *e_status = new QLineEdit(box);

	QLabel *l_nickname = new QLabel(box);
	l_nickname->setText(i18n("Nickname"));
	e_nickname = new QLineEdit(box);

	QLabel *l_firstname = new QLabel(box);
	l_firstname->setText(i18n("First name"));
	e_firstname = new QLineEdit(box);

	QLabel *l_lastname = new QLabel(box);
	l_lastname->setText(i18n("Surname"));
	e_lastname = new QLineEdit(box);

	QLabel *l_altnick = new QLabel(box);
	l_altnick->setText(i18n("AltNick"));
	e_altnick = new QLineEdit(box);

	QLabel *l_mobile = new QLabel(box);
	l_mobile->setText(i18n("Mobile"));
	e_mobile = new QLineEdit(box);

	QLabel *l_group = new QLabel(box);
	l_group->setText(i18n("Group"));
	e_group = new QLineEdit(box);

	QLabel *l_addr = new QLabel(box);
	l_addr->setText(i18n("Address"));
	e_addr = new QLineEdit(box);
	e_addr->setReadOnly(true);

	e_nickname->setText(puser->nickname);
	e_altnick->setText(puser->altnick);
	e_firstname->setText(puser->first_name);
	e_lastname->setText(puser->last_name);
 	e_mobile->setText(puser->mobile);	
	e_group->setText(puser->group);
	
	if (puser->ip) {
		struct in_addr in;
		in.s_addr = puser->ip;
		char address[128];
		snprintf(address, sizeof(address), "%s:%d", inet_ntoa(in), puser->port);
		e_addr->setText(i18n(address));
		}
	else
		e_addr->setText(i18n("(Unknown)"));

	switch (puser->status) {
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

	if (puser->uin)
		e_uin->setText(QString::number(puser->uin));

	addTab(box, i18n("General"));
}

void UserInfo::setupTab2() {
	QVBox *box = new QVBox(this);
	box->setMargin(10);

	c_blocking = new QCheckBox(i18n("Block user"), box);
	c_blocking->setChecked(puser->blocking);

	c_offtouser = new QCheckBox(i18n("Offline to user"), box);
	c_offtouser->setChecked(puser->offline_to_user);

	c_notify = new QCheckBox(i18n("Notify about status changes"), box);
	c_notify->setChecked(puser->notify);

	addTab(box, i18n("Others"));
}

void UserInfo::writeUserlist() {
	bool yes = false;
	bool ok;
	uin_t uin;
	switch (QMessageBox::information(this, "Kadu", i18n("This will write current userlist"), i18n("OK"), i18n("Cancel"), QString::null, 0, 1)) {
		case 0: // Yes?
			fprintf(stderr, "KK UserInfo::writeUserlist() \n");
			uin = e_uin->text().toUInt(&ok);
			if (!ok) {
				QMessageBox::information(this, "Kadu", i18n("Bad UIN"), QMessageBox::Ok);
				close();
				break;
				}
			if (uin != puser->uin && userlist.containsUin(uin)) {
				QMessageBox::information(this, "Kadu", i18n("User is already in userlist"), QMessageBox::Ok);
				close();
				break;
				}
			if (sess && sess->status != GG_STATUS_NOT_AVAIL) {
				if (c_offtouser->isChecked() && !puser->offline_to_user) {
					gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
					gg_add_notify_ex(sess, uin, GG_USER_OFFLINE);
					}
				else
					if (!c_offtouser->isChecked() && puser->offline_to_user) {
						gg_remove_notify_ex(sess, puser->uin, GG_USER_OFFLINE);
						gg_add_notify_ex(sess, uin, GG_USER_NORMAL);
						}
					else
						if (c_blocking->isChecked() && !puser->blocking) {
							gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
							gg_add_notify_ex(sess, uin, GG_USER_BLOCKED);
							}
						else
							if (!c_blocking->isChecked() && puser->blocking) {
								gg_remove_notify_ex(sess, puser->uin, GG_USER_BLOCKED);
								gg_add_notify_ex(sess, uin, GG_USER_NORMAL);
								}
							else
								if (puser->anonymous)
									gg_add_notify(sess, uin);
								else {
									gg_remove_notify(sess, puser->uin);
									gg_add_notify(sess, uin);
									}
				}
			userlist.changeUserInfo(puser->altnick,
				e_firstname->text(),e_lastname->text(),
				e_nickname->text(),e_altnick->text(),
				e_mobile->text(), e_uin->text(),
				uin != puser->uin ? GG_STATUS_NOT_AVAIL : puser->status,
				c_blocking->isChecked(), c_offtouser->isChecked(),
				c_notify->isChecked(), e_group->text());
			puser->anonymous = false;
			userlist.writeToFile();
			UserBox::all_refresh();
			for (int i = 0; i < chats.count(); i++)
				if (chats[i].uins.contains(puser->uin))
					chats[i].ptr->setTitle();
			close();
			break;
		case 1:
			return;	
		}
}

