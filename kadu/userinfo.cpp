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

UserInfo::UserInfo (const QString &name, QDialog *parent,
	const QString &altnick, bool fAddUser)
: QTabDialog(parent, name), fAddUser(fAddUser) {
	resize(350,200);
	setWFlags(Qt::WDestructiveClose);

	int i = 0;
	if (fAddUser) {
		puser = NULL;
		setCaption(tr("Add user"));
		setOkButton(tr("Add"));
		}
	else {
		while (i < userlist.size() && userlist[i].altnick != altnick)
			i++;
		puser = &userlist[i];
		setCaption(tr("User info on %1").arg(altnick));
		setOkButton(tr("Update"));
		}

	setupTab1();
	setupTab2();

	setCancelButton(tr("Close"));

	disconnect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));
	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateUserlist()));
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
	e_nickname = new QLineEdit(vbox21);
	QVBox *vbox22 = new QVBox(hbox2);
	QLabel *l_altnick = new QLabel(tr("AltNick"), vbox22);
	e_altnick = new QLineEdit(vbox22);

	QHBox *hbox3 = new QHBox(box);
	hbox3->setSpacing(10);
	QVBox *vbox31 = new QVBox(hbox3);
	QLabel *l_firstname = new QLabel(tr("First name"), vbox31);
	e_firstname = new QLineEdit(vbox31);
	QVBox *vbox32 = new QVBox(hbox3);
	QLabel *l_lastname = new QLabel(tr("Surname"), vbox32);
	e_lastname = new QLineEdit(vbox32);

	QHBox *hbox4 = new QHBox(box);
	hbox4->setSpacing(10);
	QVBox *vbox41 = new QVBox(hbox4);
	QLabel *l_mobile = new QLabel(tr("Mobile"), vbox41);
	e_mobile = new QLineEdit(vbox41);
	QVBox *vbox42 = new QVBox(hbox4);
	QLabel *l_group = new QLabel(tr("Group"), vbox42);
	cb_group = new QComboBox(vbox42);
	cb_group->insertStringList(list);
	cb_group->setEditable(true);
	cb_group->setAutoCompletion(true);
	hbox4->setStretchFactor(vbox41, 1);
	hbox4->setStretchFactor(vbox42, 1);

	QHBox *hbox5 = new QHBox(box);
	hbox5->setSpacing(10);
	QVBox *vbox51 = new QVBox(hbox5);
	QLabel *l_addr = new QLabel(tr("Address IP and Port"), vbox51);
	e_addr = new QLineEdit(vbox51);
	QVBox *vbox52 = new QVBox(hbox5);
	QLabel *l_ver = new QLabel(tr("Protocol version"), vbox52);
	e_ver = new QLineEdit(vbox52);
	
	QHBox *hbox6 = new QHBox(box);
	hbox6->setSpacing(10);
	QVBox *vbox61 = new QVBox(hbox6);
	QLabel *l_dnsname = new QLabel(tr("DNS name"), vbox61);
	e_dnsname = new QLineEdit(vbox61);
	QVBox *vbox62 = new QVBox(hbox6);
	QLabel *l_email = new QLabel(tr("Email"), vbox62);
	e_email = new QLineEdit(vbox62);

	if (!userlist_sent)
		e_status->setText(tr("(Unknown)"));

	addTab(box, tr("General"));

	dns = new QDns();

	e_status->setReadOnly(true);	
	e_addr->setReadOnly(true);
	e_ver->setReadOnly(true);
	e_dnsname->setReadOnly(true);
	if (fAddUser) {
		e_status->setEnabled(false);
		e_addr->setEnabled(false);
		e_ver->setEnabled(false);
		e_dnsname->setEnabled(false);
		cb_group->setCurrentText(tr("All"));
		}
	else {
		if (puser->uin)
			e_uin->setText(QString::number(puser->uin));
		e_nickname->setText(puser->nickname);
		e_altnick->setText(puser->altnick);
		e_firstname->setText(puser->first_name);
		e_lastname->setText(puser->last_name);
		e_mobile->setText(puser->mobile);
		e_email->setText(puser->email);
		if (puser->group().isEmpty())
			cb_group->setCurrentText(tr("All"));
		else
			cb_group->setCurrentText(puser->group());
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
		dns->setLabel(puser->ip);
		dns->setRecordType(QDns::Ptr);
		connect(dns, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
		}
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
	c_offtouser = new QCheckBox(tr("Offline to user"), box);
	c_notify = new QCheckBox(tr("Notify about status changes"), box);

	if (!fAddUser) {
		c_blocking->setChecked(puser->blocking);
		c_offtouser->setChecked(puser->offline_to_user);
		c_notify->setChecked(puser->notify);
		}
	addTab(box, tr("Others"));
}

void UserInfo::accept() {
}

void UserInfo::updateUserlist() {
	bool ok;
	UserListElement e;

	kdebug("UserInfo::updateUserlist() \n");
	e.first_name = e_firstname->text();
	e.last_name = e_lastname->text();
	e.nickname = e_nickname->text();
	e.altnick = e_altnick->text();
	e.mobile = e_mobile->text();
	e.uin = e_uin->text().toUInt(&ok);
	if (!ok)
		e.uin = 0;
	if (cb_group->currentText() == tr("All"))
		e.setGroup("");
	else
		e.setGroup(cb_group->currentText());
	e.description = "";
	e.email = e_email->text();
	if (fAddUser) {
		if (e_altnick->text().length()) {
			kadu->addUser(e);
			close(true);
			}
		else
			QMessageBox::warning(this, tr("Add user problem"),
				tr("Altnick field cannot be empty."));
		}
	else {
		if (e_uin->text().length() && !e.uin) {
			QMessageBox::information(this, "Kadu",
				tr("Bad UIN"), QMessageBox::Ok);
			close();
			return;
			}
		if (e.uin && e.uin != puser->uin && userlist.containsUin(e.uin)) {
			QMessageBox::information(this, "Kadu",
				tr("User is already in userlist"), QMessageBox::Ok);
			close();
			return;
			}
		if (sess && sess->status != GG_STATUS_NOT_AVAIL) {
			if (c_offtouser->isChecked() && !puser->offline_to_user) {
				if (puser->uin)
					gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
				if (e.uin)
					gg_add_notify_ex(sess, e.uin, GG_USER_OFFLINE);
				}
			else
				if (!c_offtouser->isChecked() && puser->offline_to_user) {
					if (puser->uin)
						gg_remove_notify_ex(sess, puser->uin, GG_USER_OFFLINE);
					if (e.uin)
						gg_add_notify_ex(sess, e.uin, GG_USER_NORMAL);
					}
				else
					if (c_blocking->isChecked() && !puser->blocking) {
						if (puser->uin)
							gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
						if (e.uin)
							gg_add_notify_ex(sess, e.uin, GG_USER_BLOCKED);
						}
					else
						if (!c_blocking->isChecked() && puser->blocking) {
							if (puser->uin)
								gg_remove_notify_ex(sess, puser->uin, GG_USER_BLOCKED);
							if (e.uin)
								gg_add_notify_ex(sess, e.uin, GG_USER_NORMAL);
							}
						else
							if (puser->anonymous) {
								if (e.uin)
									gg_add_notify(sess, e.uin);
								}
							else {
								if (puser->uin)
									gg_remove_notify(sess, puser->uin);
								if (e.uin)
									gg_add_notify(sess, e.uin);
								}
			}
		if (e.uin == puser->uin)
			e.status = puser->status;
		e.image_size = puser->image_size;
		e.blocking = c_blocking->isChecked();
		e.offline_to_user = c_offtouser->isChecked();
		e.notify = c_notify->isChecked();
		userlist.changeUserInfo(puser->altnick, e);
		userlist.writeToFile();
		UserBox::all_refresh();
		for (int i = 0; i < chats.count(); i++)
			if (chats[i].uins.contains(puser->uin))
				chats[i].ptr->setTitle();
		close();
		}
}

