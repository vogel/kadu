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

//
#include "gadu.h"
#include "kadu.h"
#include "userinfo.h"
#include "chat.h"
#include "debug.h"
#include "tabbar.h"
//

CreateNotifier UserInfo::createNotifier;

UserInfo::UserInfo(const QString &name, QDialog *parent, const QString &altnick, bool fAddUser)
: fAddUser(fAddUser) 
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	
	unsigned int i = 0;
	if (fAddUser)
	{
		puser = NULL;
		setCaption(tr("Add user"));
	}
	else 
	{
		while (i < userlist.size() && userlist[i].altnick != altnick)
			i++;
		puser = &userlist[i];
		setCaption(tr("User info on %1").arg(altnick));
	}
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left);
	QWidget *w_icoblankwidget=new QWidget(left);
	w_icoblankwidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("ManageUsersWindowIcon"));
	l_info->setText(tr("This dialog box allows you to view and edit information about the selected contact."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	tw_main = new QTabWidget(center);
	
	// create our Tabs
	setupTab1();
	setupTab2();

	// buttons
	QHBox *bottom=new QHBox(center);
	QWidget *w_blankwidget=new QWidget(bottom);
	bottom->setSpacing(5);
	w_blankwidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	if (fAddUser)
		pb_addapply = new QPushButton(icons_manager.loadIcon("AddUserButton"), tr("Add"), bottom, "add");
	else
		pb_addapply = new QPushButton(icons_manager.loadIcon("UpdateUserButton"), tr("Update"), bottom, "update");
	
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));	
	connect(pb_addapply, SIGNAL(clicked()), this, SLOT(updateUserlist()));
	
	createNotifier.notify(this);
	
	loadGeometry(this, "General", "ManageUsersDialogGeometry", 0, 0, 450, 400);
	kdebugf2();
}

void UserInfo::setUserInfo(UserListElement &ule) {
	e_firstname->setText(ule.first_name);
	e_lastname->setText(ule.last_name);
	e_nickname->setText(ule.nickname);
	e_altnick->setText(ule.altnick);
	e_mobile->setText(ule.mobile);
	if (ule.uin)
		e_uin->setText(QString::number(ule.uin));
	e_email->setText(ule.email);
}

void UserInfo::setupTab1()
{
	kdebugf();
	// our QVGroupBox
	vgb_general = new QVGroupBox(tw_main);
	vgb_general->setFrameStyle(QFrame::NoFrame);
	// end our QGroupBox
	
	tw_main->addTab(vgb_general, tr("General"));
	
	// info panel
	
	// UIN and STATUS
	QHBox *hb_uinstate = new QHBox(vgb_general);
	QVBox *vb_uin = new QVBox(hb_uinstate);
	QVBox *vb_state = new QVBox(hb_uinstate);
	hb_uinstate->setSpacing(3);
	vb_uin->setSpacing(3);
	vb_state->setSpacing(3);
	
	new QLabel(tr("Uin"), vb_uin);
	e_uin = new QLineEdit(vb_uin);
	new QLabel(tr("Status"), vb_state);
	QLineEdit *e_status = new QLineEdit(vb_state);
	// end UIN and STATUS
	
	// Nick and Disp. nick
	QHBox *hb_dispnick = new QHBox(vgb_general);
	QVBox *vb_nick = new QVBox(hb_dispnick);
	QVBox *vb_disp = new QVBox(hb_dispnick);
	hb_dispnick->setSpacing(3);
	vb_nick->setSpacing(3);
	vb_disp->setSpacing(3);
	
	new QLabel(tr("Nickname"), vb_nick);
	e_nickname = new QLineEdit(vb_nick);
	new QLabel(tr("AltNick"), vb_disp);
	e_altnick = new QLineEdit(vb_disp);
	// end Nick and Disp. nick
	
	// Name and Surname
	QHBox *hb_namesurname = new QHBox(vgb_general);
	QVBox *vb_name = new QVBox(hb_namesurname);
	QVBox *vb_surname = new QVBox(hb_namesurname);
	hb_namesurname->setSpacing(3);
	vb_name->setSpacing(3);
	vb_surname->setSpacing(3);
	
	new QLabel(tr("First name"), vb_name);
	e_firstname = new QLineEdit(vb_name);
	new QLabel(tr("Surname"), vb_surname);
	e_lastname = new QLineEdit(vb_surname);
	// end Name and Surname
	
	// Mobile and Group
	QHBox *hb_mobilegroup = new QHBox(vgb_general);
	QVBox *vb_mobile = new QVBox(hb_mobilegroup);
	QVBox *vb_group = new QVBox(hb_mobilegroup);
	hb_mobilegroup->setSpacing(3);
	vb_mobile->setSpacing(3);
	vb_group->setSpacing(3);
	
	// get available groups
	QStringList list;
	for (int i=0; i < kadu->groupBar()->count(); i++)
		list << kadu->groupBar()->tabAt(i)->text();
	// end get available groups
	
	new QLabel(tr("Mobile"), vb_mobile);
	e_mobile = new QLineEdit(vb_mobile);
	new QLabel(tr("Group"), vb_group);
	cb_group = new QComboBox(vb_group);
	cb_group->insertStringList(list);
	cb_group->setEditable(true);
	cb_group->setAutoCompletion(true);
	hb_mobilegroup->setStretchFactor(vb_mobile, 1);
	hb_mobilegroup->setStretchFactor(vb_group, 1);
	// end Mobile and Group
	
	// IP and Protocol Version
	QHBox *hb_ipprotversion = new QHBox(vgb_general);
	QVBox *vb_ip = new QVBox(hb_ipprotversion);
	QVBox *vb_protversion = new QVBox(hb_ipprotversion);
	hb_ipprotversion->setSpacing(3);
	vb_ip->setSpacing(3);
	vb_protversion->setSpacing(3);
	
	new QLabel(tr("Address IP and Port"), vb_ip);
	e_addr = new QLineEdit(vb_ip);
	new QLabel(tr("Protocol version"), vb_protversion);
	e_ver = new QLineEdit(vb_protversion);
	// end IP and Protocol Version
	
	// DNS and Email
	QHBox *hb_dnsemail = new QHBox(vgb_general);
	QVBox *vb_dns = new QVBox(hb_dnsemail);
	QVBox *vb_email = new QVBox(hb_dnsemail);
	hb_dnsemail->setSpacing(3);
	vb_dns->setSpacing(3);
	vb_email->setSpacing(3);
	
	new QLabel(tr("DNS name"), vb_dns);
	e_dnsname = new QLineEdit(vb_dns);
	new QLabel(tr("Email"), vb_email);
	e_email = new QLineEdit(vb_email);
	// end DNS and Email
	
	if (!userlist_sent)
		e_status->setText(tr("(Unknown)"));
		
	QString s_temp;
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
			s_temp.sprintf("0x%02x", puser->version & 0x0000ffff);
			e_ver->setText(s_temp);
			}
		else
			e_ver->setText(tr("(Unknown)"));

		switch (puser->status) {
			case GG_STATUS_AVAIL:
				e_status->setText(tr("Online"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("Online"));
				break;
			case GG_STATUS_AVAIL_DESCR:
				e_status->setText(tr("Online (d.)"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("OnlineWithDescription"));
				break;
			case GG_STATUS_NOT_AVAIL:
				e_status->setText(tr("Offline"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("Offline"));
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				e_status->setText(tr("Offline (d.)"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("OfflineWithDescription"));
				break;
			case GG_STATUS_BUSY:
				e_status->setText(tr("Busy"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("Busy"));
				break;
			case GG_STATUS_BUSY_DESCR:
				e_status->setText(tr("Busy (d.)"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("BusyWithDescription"));
				break;
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE2:
				e_status->setText(tr("Invisible"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("Invisible"));
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				e_status->setText(tr("Invisible (d.)"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("InvisibleWithDescription"));
				break;
			case GG_STATUS_BLOCKED:
				e_status->setText(tr("Blocks us"));
				tw_main->setTabIconSet(vgb_general, icons_manager.loadIcon("Blocking"));
				break;
			}
		dns->setLabel(puser->ip);
		dns->setRecordType(QDns::Ptr);
		connect(dns, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	}
	kdebugf2();
}

void UserInfo::setupTab2()
{
	kdebugf();
	// Misc options
	QVGroupBox *vgb_others = new QVGroupBox(vgb_general);
	vgb_others->setFrameStyle(QFrame::NoFrame);
	
	tw_main->addTab(vgb_others, tr("Others"));
	
	c_blocking = new QCheckBox(tr("Block user"), vgb_others);
	c_offtouser = new QCheckBox(tr("Offline to user"), vgb_others);
	c_notify = new QCheckBox(tr("Notify about status changes"), vgb_others);

	if (!fAddUser) {
		c_blocking->setChecked(puser->blocking);
		c_offtouser->setChecked(puser->offline_to_user);
		c_notify->setChecked(puser->notify);
		}
	else
		c_notify->setChecked(true);
	// end Misc options	
	kdebugf2();
}

UserInfo::~UserInfo() 
{
	kdebugf();
	saveGeometry(this, "General", "ManageUsersDialogGeometry");
	delete dns;
	kdebugf2();
}

void UserInfo::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void UserInfo::resultsReady() {
	e_dnsname->setText(dns->hostNames()[0]);
}

void UserInfo::addNewUser(UserListElement& e)
{
	kdebugf();
	bool uin_exist = e.uin && (userlist.containsUin(e.uin));
	if (uin_exist)
	{
		puser = &userlist.byUin(e.uin);
		if (puser->anonymous)
		{
			changeUserData(e);
			return;
		}	
	}
	if (!e_altnick->text().length())
	{
		QMessageBox::warning(this, tr("Add user problem"),
			tr("Altnick field cannot be empty."));
		return;
	}
	if (userlist.containsAltNick(e_altnick->text()) || uin_exist)
	{
		QMessageBox::information(this, "Kadu",
			tr("User is already in userlist"), QMessageBox::Ok);
		return;
	}
	userlist.addUser(e);
	close(true);
	kdebugf2();
}

void UserInfo::changeUserData(UserListElement& e)
{
	kdebugf();
	if (e_uin->text().length() && !e.uin) {
		QMessageBox::information(this, "Kadu", tr("Bad UIN"), QMessageBox::Ok);
		close();
		return;
	}

	if (!e_altnick->text().length()) {
		QMessageBox::warning(this, tr("Add user problem"), tr("Altnick field cannot be empty."));
		close();
		return;
	}

	if ((e.uin && e.uin != puser->uin && userlist.containsUin(e.uin)) ||
		 (e.altnick.lower() != puser->altnick.lower() && userlist.containsAltNick(e.altnick))) {
		QMessageBox::information(this, "Kadu", tr("User is already in userlist"), QMessageBox::Ok);
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
		else if (!c_offtouser->isChecked() && puser->offline_to_user) {
			if (puser->uin)
				gg_remove_notify_ex(sess, puser->uin, GG_USER_OFFLINE);
			if (e.uin)
				gg_add_notify_ex(sess, e.uin, GG_USER_NORMAL);
		}
		else if (c_blocking->isChecked() && !puser->blocking) {
			if (puser->uin)
				gg_remove_notify_ex(sess, puser->uin, GG_USER_NORMAL);
			if (e.uin)
				gg_add_notify_ex(sess, e.uin, GG_USER_BLOCKED);
		}
		else if (!c_blocking->isChecked() && puser->blocking) {
			if (puser->uin)
				gg_remove_notify_ex(sess, puser->uin, GG_USER_BLOCKED);
			if (e.uin)
				gg_add_notify_ex(sess, e.uin, GG_USER_NORMAL);
		}
		else if (puser->anonymous) {
			if (e.uin)
				gg_add_notify(sess, e.uin);
		}
		else if (e.uin != puser->uin) {
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
	if (!kadu->userbox()->containsAltNick(e.altnick))
	{
		kadu->userbox()->addUser(e.altnick);
		UserBox::all_refresh();
	}
	chat_manager->refreshTitlesForUin(puser->uin);
	close(true);
	kdebugf2();
}

void UserInfo::updateUserlist() {
	bool ok;
	UserListElement e;

	kdebugf();
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
	if (fAddUser)
		addNewUser(e);
	else
		changeUserData(e);
	kdebugf2();
}
