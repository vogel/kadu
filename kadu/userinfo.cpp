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
#include "debug.h"
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

	QLabel *l_uin = new QLabel(i18n("Uin"),box);
	e_uin = new QLineEdit(box);

	QLabel *l_status = new QLabel(i18n("Status"),box);
	QLineEdit *e_status = new QLineEdit(box);

	QLabel *l_nickname = new QLabel(i18n("Nickname"),box);
	e_nickname = new QLineEdit(puser->nickname,box);

	QLabel *l_firstname = new QLabel(i18n("First name"),box);
	e_firstname = new QLineEdit(puser->first_name,box);

	QLabel *l_lastname = new QLabel(i18n("Surname"),box);
	e_lastname = new QLineEdit(puser->last_name,box);

	QLabel *l_altnick = new QLabel(i18n("AltNick"),box);
	e_altnick = new QLineEdit(box);

	QLabel *l_mobile = new QLabel(i18n("Mobile"),box);
	e_mobile = new QLineEdit(puser->mobile,box);

	QLabel *l_group = new QLabel(i18n("Group"),box);
	e_group = new QLineEdit(puser->group,box);

	QLabel *l_addr = new QLabel(i18n("Address"),box);
	e_addr = new QLineEdit(box);
	e_addr->setReadOnly(true);
	
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
			kdebug("UserInfo::writeUserlist() \n");
			uin = e_uin->text().toUInt(&ok);
			if (!ok && e_uin->text().length()) {
				QMessageBox::information(this, "Kadu", i18n("Bad UIN"), QMessageBox::Ok);
				close();
				break;
				}
			if (uin && uin != puser->uin && userlist.containsUin(uin)) {
				QMessageBox::information(this, "Kadu", i18n("User is already in userlist"), QMessageBox::Ok);
				close();
				break;
				}
			if (sess && sess->status != GG_STATUS_NOT_AVAIL) {
				if (c_offtouser->isChecked() && !puser->offline_to_user) {
					if (puser->uin)
						gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
					if (uin)
						gg_add_notify_ex(sess, uin, GG_USER_OFFLINE);
					}
				else
					if (!c_offtouser->isChecked() && puser->offline_to_user) {
						if (puser->uin)
							gg_remove_notify_ex(sess, puser->uin, GG_USER_OFFLINE);
						if (uin)
							gg_add_notify_ex(sess, uin, GG_USER_NORMAL);
						}
					else
						if (c_blocking->isChecked() && !puser->blocking) {
							if (puser->uin)
								gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
							if (uin)
								gg_add_notify_ex(sess, uin, GG_USER_BLOCKED);
							}
						else
							if (!c_blocking->isChecked() && puser->blocking) {
								if (puser->uin)
									gg_remove_notify_ex(sess, puser->uin, GG_USER_BLOCKED);
								if (uin)
									gg_add_notify_ex(sess, uin, GG_USER_NORMAL);
								}
							else
								if (puser->anonymous) {
									if (uin)
										gg_add_notify(sess, uin);
									}
								else {
									if (puser->uin)
										gg_remove_notify(sess, puser->uin);
									if (uin)
										gg_add_notify(sess, uin);
									}
				}
			userlist.changeUserInfo(puser->altnick,
				e_firstname->text(),e_lastname->text(),
				e_nickname->text(),e_altnick->text(),
				e_mobile->text(), e_uin->text().length() ? e_uin->text() : QString("0"),
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

