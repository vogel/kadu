/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvbox.h>

//
#include "kadu.h"
#include "userinfo.h"
#include "chat.h"
#include "debug.h"
#include "userbox.h"
#include "message_box.h"
//

CreateNotifier UserInfo::createNotifier;

UserInfo::UserInfo(const QString &altnick, bool addUser, QDialog* parent, const char *name)
	: QHBox(parent, name), addUser(addUser)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose|Qt::WShowModal);

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
	l_info->setText(tr("This dialog box allows you to view and edit information about the selected contact."));
	l_info->setAlignment(Qt::WordBreak);

	if (addUser)
	{
		puser = NULL;
		setCaption(tr("Add user"));
		l_icon->setPixmap(icons_manager.loadIcon("AddUserWindowIcon"));
	}
	else
	{
		for (UserList::Iterator i = userlist.begin(); i != userlist.end(); ++i)
			if ((*i).altNick() == altnick)
			{
				puser = &(*i);
				break;
			}
		setCaption(tr("User info on %1").arg(altnick));

		l_icon->setPixmap(icons_manager.loadIcon("ManageUsersWindowIcon"));
	}

	// end create main QLabel widgets (icon and app info)

	tw_main = new QTabWidget(center);

	// create our Tabs
	setupTab1();
	setupTab2();
	setupTab3();

	// buttons
	QHBox *bottom=new QHBox(center);
	QWidget *w_blankwidget=new QWidget(bottom);
	bottom->setSpacing(5);
	w_blankwidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	if (addUser)
		pb_addapply = new QPushButton(icons_manager.loadIcon("AddUserButton"), tr("Add"), bottom, "add");
	else
		pb_addapply = new QPushButton(icons_manager.loadIcon("UpdateUserButton"), tr("Update"), bottom, "update");

	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_addapply, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	createNotifier.notify(this);

	loadGeometry(this, "General", "ManageUsersDialogGeometry", 0, 0, 380, 450);
	kdebugf2();
}

void UserInfo::setUserInfo(UserListElement &ule) {
	e_firstname->setText(ule.firstName());
	e_lastname->setText(ule.lastName());
	e_nickname->setText(ule.nickName());
	e_altnick->setText(ule.altNick());
	e_mobile->setText(ule.mobile());
	if (ule.uin())
		e_uin->setText(QString::number(ule.uin()));
	e_email->setText(ule.email());
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

	// Mobile and Email
	QHBox *hb_mobileemail = new QHBox(vgb_general);
	QVBox *vb_mobile = new QVBox(hb_mobileemail);
	QVBox *vb_email = new QVBox(hb_mobileemail);
	hb_mobileemail->setSpacing(3);
	vb_mobile->setSpacing(3);
	vb_email->setSpacing(3);

	new QLabel(tr("Mobile"), vb_mobile);
	e_mobile = new QLineEdit(vb_mobile);
	new QLabel(tr("Email"), vb_email);
	e_email = new QLineEdit(vb_email);
	// end Mobile and Email

	// IP and DNS
	QHBox *hb_ipdns = new QHBox(vgb_general);
	QVBox *vb_ip = new QVBox(hb_ipdns);
	QVBox *vb_dns = new QVBox(hb_ipdns);
	hb_ipdns->setSpacing(3);
	vb_ip->setSpacing(3);
	vb_dns->setSpacing(3);

	new QLabel(tr("Address IP and Port"), vb_ip);
	e_addr = new QLineEdit(vb_ip);
	new QLabel(tr("DNS name"), vb_dns);
	e_dnsname = new QLineEdit(vb_dns);
	// end IP and DNS

	// Protocol Version
	QHBox *hb_protversion = new QHBox(vgb_general);
	QVBox *vb_protversion = new QVBox(hb_protversion);
	QVBox *vb_empty = new QVBox(hb_protversion);
	hb_protversion->setSpacing(3);
	vb_protversion->setSpacing(3);
	vb_empty->setSpacing(3);
	new QLabel(tr("Protocol version"), vb_protversion);
	e_ver = new QLineEdit(vb_protversion);

	hb_protversion->setStretchFactor(vb_protversion, 1);
	hb_protversion->setStretchFactor(vb_empty, 1);
	// end Protocol Version

	if (!gadu->userListSent())
		e_status->setText(tr("(Unknown)"));

	QString s_temp;
	dns = new QDns();

	e_status->setReadOnly(true);
	e_addr->setReadOnly(true);
	e_ver->setReadOnly(true);
	e_dnsname->setReadOnly(true);
	if (addUser)
	{
		e_status->setEnabled(false);
		e_addr->setEnabled(false);
		e_ver->setEnabled(false);
		e_dnsname->setEnabled(false);
	}
	else
	{
		if (puser->uin())
			e_uin->setText(QString::number(puser->uin()));
		e_nickname->setText(puser->nickName());
		e_altnick->setText(puser->altNick());
		e_firstname->setText(puser->firstName());
		e_lastname->setText(puser->lastName());
		e_mobile->setText(puser->mobile());
		e_email->setText(puser->email());
		if (puser->ip().ip4Addr())
			e_addr->setText(puser->ip().toString());
		else
			e_addr->setText(tr("(Unknown)"));
		if (puser->port())
			e_addr->setText(e_addr->text()+":"+QString::number(puser->port()));
		else
			e_addr->setText(e_addr->text()+":"+tr("(Unknown)"));

		if (puser->version())
		{
			s_temp.sprintf("0x%02x", puser->version() & 0x0000ffff);
			e_ver->setText(s_temp);
		}
		else
			e_ver->setText(tr("(Unknown)"));

		e_status->setText(tr(puser->status().name()));
		tw_main->setTabIconSet(vgb_general, puser->status().pixmap());
		if (!(puser->ip() == QHostAddress()))
		{
			dns->setLabel(puser->ip());
			dns->setRecordType(QDns::Ptr);
			connect(dns, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
		}
	}
	kdebugf2();
}

void UserInfo::setupTab2()
{
	kdebugf();
	QVGroupBox *groupsTab = new QVGroupBox(vgb_general);
	groupsTab->setFrameStyle(QFrame::NoFrame);

	tw_main->addTab(groupsTab, tr("Groups"));

	// get available groups
	QStringList list;
	for (int i=0; i < kadu->groupBar()->count(); ++i)
		list << kadu->groupBar()->tabAt(i)->text();
	list.remove(tr("All"));
	// end get available groups

	QStringList groupsList;
	if (!addUser)
		groupsList=QStringList::split(",", puser->group());

	groupsBox = new QVBox(groupsTab);
	groupsBox->setSpacing(3);

	for (QStringList::iterator it=list.begin(); it!=list.end(); ++it)
	{
		QCheckBox *checkBox=new QCheckBox(*it, groupsBox);
		checkBox->setChecked(groupsList.contains(*it));
		groups.append(checkBox);
	}

	//zobacz komentarz w newGroupClicked()
	for (int i=0; i<10; ++i)
	{
		QCheckBox *box=new QCheckBox(groupsBox);
		box->setChecked(true);
		box->hide();
		hiddenCheckBoxes.append(box);
	}

	newGroup=new QLineEdit(groupsTab);
	QPushButton *addNewGroup=new QPushButton(tr("Add new group"), groupsTab);
	connect(addNewGroup, SIGNAL(clicked()), this, SLOT(newGroupClicked()));
	connect(newGroup, SIGNAL(returnPressed()), this, SLOT(newGroupClicked()));

	kdebugf2();
}

void UserInfo::newGroupClicked()
{
	kdebugf();
	QString groupName=newGroup->text();
	if (groupName.isEmpty())
		return;
	if (groupName.contains(","))
	{
		MessageBox::msg(tr("'%1' is prohibited").arg(','), true);
		return;
	}
	if (groupName.contains(";"))
	{
		MessageBox::msg(tr("'%1' is prohibited").arg(';'), true);
		return;
	}
	if (groupName==tr("All"))
	{
		MessageBox::msg(tr("This group already exists!"), true);
		return;
	}
	for (QValueList<QCheckBox *>::iterator it=groups.begin(); it!=groups.end(); ++it)
		if ((*it)->text()==groupName)
		{
			MessageBox::wrn(tr("This group already exists!"), true);
			return;
		}
//	niestety ten 2-linijkowy kod nie dzia³a - nie wiem dlaczego, ale chêtnie siê dowiem
//	w ka¿dym razie dlatego trzeba by³o stworzyæ wcze¶niej kilka checkboksów i je ukryæ...
//	a teraz po trochu je pokazujemy :)
//	QCheckBox *box=new QCheckBox(groupName, groupsBox);
//	box->setChecked(true);

	if (hiddenCheckBoxes.size()==0)
	{
		MessageBox::msg(tr("You can't add so many groups at one stroke. Close this dialog and open one more time."), true);
		return;
	}
	QCheckBox *box=hiddenCheckBoxes.first();
	hiddenCheckBoxes.pop_front();
	box->setText(groupName);
	box->show();

	groups.append(box);
	kdebugf2();
}

void UserInfo::setupTab3()
{
	kdebugf();
	// Misc options
	QVGroupBox *vgb_others = new QVGroupBox(vgb_general);
	vgb_others->setFrameStyle(QFrame::NoFrame);

	tw_main->addTab(vgb_others, tr("Others"));

	c_blocking = new QCheckBox(tr("Block user"), vgb_others);
	c_offtouser = new QCheckBox(tr("Offline to user"), vgb_others);
	c_notify = new QCheckBox(tr("Notify about status changes"), vgb_others);

	if (!config_file.readBoolEntry("General", "PrivateStatus"))
		c_offtouser->setEnabled(false);

	if (addUser)
		c_notify->setChecked(true);
	else
	{
		c_blocking->setChecked(puser->blocking());
		c_offtouser->setChecked(puser->offlineTo());
		c_notify->setChecked(puser->notify());
	}
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
	bool uin_exist = e.uin() && (userlist.containsUin(e.uin()));
	if (uin_exist)
	{
		puser = &userlist.byUin(e.uin());
		if (puser->isAnonymous())
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
	userlist.writeToFile();
	close(true);

	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs"))
	{
		kadu->userbox()->addUser(e.altNick());
		kadu->userbox()->refresh();
	}

	kdebugf2();
}

void UserInfo::changeUserData(UserListElement& e)
{
	kdebugf();
	if (e_uin->text().length() && !e.uin())
	{
		QMessageBox::information(this, "Kadu", tr("Bad UIN"), QMessageBox::Ok);
		close();
		return;
	}

	if (!e_altnick->text().length())
	{
		QMessageBox::warning(this, tr("Add user problem"), tr("Altnick field cannot be empty."));
		close();
		return;
	}

	if ((e.uin() && e.uin() != puser->uin() && userlist.containsUin(e.uin())) ||
		(e.altNick().lower() != puser->altNick().lower() && userlist.containsAltNick(e.altNick())))
	{
		QMessageBox::information(this, "Kadu", tr("User is already in userlist"), QMessageBox::Ok);
		close();
		return;
	}


	if (e.uin() == puser->uin())
		e.status().setStatus(puser->status());
	e.setMaxImageSize(puser->maxImageSize());
	e.ip()=puser->ip();
	e.setPort(puser->port());
	e.setDnsName(puser->dnsName());

	userlist.changeUserInfo(puser->altNick(), e);
	userlist.writeToFile();

	chat_manager->refreshTitlesForUin(puser->uin());
	close(true);
	kdebugf2();
}

void UserInfo::updateUserlist()
{
	bool ok;
	UserListElement e;

	kdebugf();
	e.setFirstName(e_firstname->text());
	e.setLastName(e_lastname->text());
	e.setNickName(e_nickname->text());
	e.setAltNick(e_altnick->text());
	e.setMobile(e_mobile->text());
	e.setUin(e_uin->text().toUInt(&ok));
	if (!ok)
		e.setUin(0);

	QStringList l;
	for (QValueList<QCheckBox *>::iterator it=groups.begin(); it!=groups.end(); ++it)
		if ((*it)->isChecked())
			l.append((*it)->text());
	e.setGroup(l.join(","));

	e.setEmail(e_email->text());
	e.setNotify(c_notify->isChecked());
	e.setOfflineTo(c_offtouser->isChecked());
	e.setBlocking(c_blocking->isChecked());

	if (addUser)
		addNewUser(e);
	else
		changeUserData(e);
	kdebugf2();
}
