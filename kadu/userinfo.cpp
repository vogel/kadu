/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmessagebox.h>
#include <qvbox.h>
#include <qhbox.h>

//
#include "kadu.h"
#include "userinfo.h"
#include "chat.h"
#include "debug.h"
#include "tabbar.h"
//

UserInfo::UserInfo (const QString &name, QDialog *parent , const QString &altnick)
: QTabDialog(parent, name) {
	resize(350,200);
	setCaption(tr("User info on %1").arg(altnick));
	setWFlags(Qt::WDestructiveClose);

	int i = 0;
 	while (i < userlist.size() && userlist[i].altnick != altnick)
		i++;
	puser = &userlist[i];

	setupTab1();
	setupTab2();

	setOkButton(tr("Write userlist"));
	setCancelButton(tr("Close"));

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(writeUserlist()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
}

void UserInfo::setupTab1() {
	QString tmp;
	QStringList list;
	for (int i=0; i < kadu->group_bar->count(); i++)
		list << kadu->group_bar->tabAt(i)->text();

	QVBox *box = new QVBox(this);
	box->setMargin(10);

	QHBox *hbox1 = new QHBox(box);
	hbox1->setSpacing(10);
	QVBox *vbox11 = new QVBox(hbox1);
	QLabel *l_uin = new QLabel(tr("Uin"), vbox11);
	e_uin = new QLineEdit(vbox11);
	QVBox *vbox12 = new QVBox(hbox1);
	QLabel *l_status = new QLabel(tr("Status"), vbox12);
	QLineEdit *e_status = new QLineEdit(vbox12);

	QHBox *hbox2 = new QHBox(box);
	hbox2->setSpacing(10);
	QVBox *vbox21 = new QVBox(hbox2);
	QLabel *l_nickname = new QLabel(tr("Nickname"), vbox21);
	e_nickname = new QLineEdit(puser->nickname, vbox21);
	QVBox *vbox22 = new QVBox(hbox2);
	QLabel *l_altnick = new QLabel(tr("AltNick"), vbox22);
	e_altnick = new QLineEdit(puser->altnick, vbox22);

	QHBox *hbox3 = new QHBox(box);
	hbox3->setSpacing(10);
	QVBox *vbox31 = new QVBox(hbox3);
	QLabel *l_firstname = new QLabel(tr("First name"), vbox31);
	e_firstname = new QLineEdit(puser->first_name, vbox31);
	QVBox *vbox32 = new QVBox(hbox3);
	QLabel *l_lastname = new QLabel(tr("Surname"), vbox32);
	e_lastname = new QLineEdit(puser->last_name, vbox32);

	QHBox *hbox4 = new QHBox(box);
	hbox4->setSpacing(10);
	QVBox *vbox41 = new QVBox(hbox4);
	QLabel *l_mobile = new QLabel(tr("Mobile"), vbox41);
	e_mobile = new QLineEdit(puser->mobile, vbox41);
	QVBox *vbox42 = new QVBox(hbox4);
	QLabel *l_group = new QLabel(tr("Group"), vbox42);
	cb_group = new QComboBox(vbox42);
	cb_group->insertStringList(list);
	if (puser->group().isEmpty())
		cb_group->setCurrentText(tr("All"));
	else
		cb_group->setCurrentText(puser->group());
	cb_group->setEditable(true);
	cb_group->setAutoCompletion(true);
	hbox4->setStretchFactor(vbox41, 1);
	hbox4->setStretchFactor(vbox42, 1);

	QHBox *hbox5 = new QHBox(box);
	hbox5->setSpacing(10);
	QVBox *vbox51 = new QVBox(hbox5);
	QLabel *l_addr = new QLabel(tr("Address IP and Port"), vbox51);
	e_addr = new QLineEdit(vbox51);
	e_addr->setReadOnly(true);
	QVBox *vbox52 = new QVBox(hbox5);
	QLabel *l_ver = new QLabel(tr("Protocol version"), vbox52);
	e_ver = new QLineEdit(vbox52);
	e_ver->setReadOnly(true);
	
	QHBox *hbox6 = new QHBox(box);
	hbox6->setSpacing(10);
	QVBox *vbox61 = new QVBox(hbox6);
	QLabel *l_dnsname = new QLabel(tr("DNS name"), vbox61);
	e_dnsname = new QLineEdit(vbox61);
	e_dnsname->setReadOnly(true);
	QVBox *vbox62 = new QVBox(hbox6);
	QLabel *l_email = new QLabel(tr("Email"), vbox62);
	e_email = new QLineEdit(puser->email, vbox62);

	if (puser->ip.ip4Addr())
		e_addr->setText(puser->ip.toString());
	else
		e_addr->setText(tr("(Unknown)"));
	if (puser->port)
		e_addr->setText(e_addr->text()+":"+QString::number(puser->port));
	else
		e_addr->setText(e_addr->text()+":"+tr("(Unknown)"));

	if (puser->version) {
		tmp.sprintf("0x%02x", puser->version & 0x0000ffff);
		e_ver->setText(tmp);
		}
	else
		e_ver->setText(tr("(Unknown)"));

	switch (puser->status) {
		case GG_STATUS_AVAIL:
			e_status->setText(tr("Online")); break;
		case GG_STATUS_AVAIL_DESCR:
			e_status->setText(tr("Online (d.)")); break;
		case GG_STATUS_NOT_AVAIL:
			e_status->setText(tr("Offline")); break;
		case GG_STATUS_NOT_AVAIL_DESCR:
			e_status->setText(tr("Offline (d.)")); break;
		case GG_STATUS_BUSY:
			e_status->setText(tr("Busy")); break;
		case GG_STATUS_BUSY_DESCR:
			e_status->setText(tr("Busy (d.)")); break;
		case GG_STATUS_INVISIBLE2:
			e_status->setText(tr("Invisible")); break;
		case GG_STATUS_INVISIBLE_DESCR:
			e_status->setText(tr("Invisible (d.)")); break;
		case GG_STATUS_BLOCKED:
			e_status->setText(tr("Blocks us")); break;
		}

	if (!userlist_sent)
		e_status->setText(tr("(Unknown)"));

	e_status->setReadOnly(true);

	if (puser->uin)
		e_uin->setText(QString::number(puser->uin));

	addTab(box, tr("General"));

	dns = new QDns(puser->ip);
	connect(dns, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
}

UserInfo::~UserInfo() {
	delete dns;
	kdebug("UserInfo::~UserInfo()\n");
}

void UserInfo::resultsReady() {
	e_dnsname->setText(dns->hostNames()[0]);
}

void UserInfo::setupTab2() {
	QVBox *box = new QVBox(this);
	box->setMargin(10);

	c_blocking = new QCheckBox(tr("Block user"), box);
	c_blocking->setChecked(puser->blocking);

	c_offtouser = new QCheckBox(tr("Offline to user"), box);
	c_offtouser->setChecked(puser->offline_to_user);

	c_notify = new QCheckBox(tr("Notify about status changes"), box);
	c_notify->setChecked(puser->notify);

	addTab(box, tr("Others"));
}

void UserInfo::writeUserlist() {
	bool yes = false;
	bool ok;
	uin_t uin;
	switch (QMessageBox::information(this, "Kadu", tr("This will write current userlist"), tr("OK"), tr("Cancel"), QString::null, 0, 1)) {
		case 0: // Yes?
			kdebug("UserInfo::writeUserlist() \n");
			uin = e_uin->text().toUInt(&ok);
			if (!ok && e_uin->text().length()) {
				QMessageBox::information(this, "Kadu", tr("Bad UIN"), QMessageBox::Ok);
				close();
				break;
				}
			if (uin && uin != puser->uin && userlist.containsUin(uin)) {
				QMessageBox::information(this, "Kadu", tr("User is already in userlist"), QMessageBox::Ok);
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
				puser->image_size,
				c_blocking->isChecked(), c_offtouser->isChecked(),
				c_notify->isChecked(), cb_group->currentText(), e_email->text());
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

