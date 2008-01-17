/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qdns.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "groups_manager.h"
#include "icons_manager.h"
#include "message_box.h"
#include "userinfo.h"
#include "userlist.h"

CreateNotifier UserInfo::createNotifier;

void UserInfo::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

UserInfo::UserInfo(UserListElement user, QWidget* parent, const char *name)
	: QHBox(parent, name, WType_Dialog | WShowModal | WDestructiveClose), User(user),
	e_firstname(0), e_lastname(0), e_nickname(0), e_altnick(0), e_mobile(0), e_uin(0),
	e_addr(0), e_ver(0), e_email(0), e_dnsname(0), c_blocking(0), c_offtouser(0),
	c_notify(0), pb_addapply(0), tw_main(0), vgb_general(0), dns(0), groups(),
	hiddenCheckBoxes(), newGroup(0), groupsBox(0), layoutHelper(new LayoutHelper())
{
	kdebugf();
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QVBox *left = new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *w_icoblankwidget = new QWidget(left);
	w_icoblankwidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center = new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_info->setText(tr("This dialog box allows you to view and edit information about the selected contact."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	tw_main = new QTabWidget(center);

	// create our Tabs
	setupTab1();
	setupTab2();
	setupTab3();

	// create buttons and fill icon and app info
	QHBox *bottom = new QHBox(center);
	QWidget *w_blankwidget = new QWidget(bottom);
	bottom->setSpacing(5);
	w_blankwidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	if (!userlist->contains(User, FalseForAnonymous))
	{
		setCaption(tr("Add user"));
		l_icon->setPixmap(icons_manager->loadIcon("AddUserWindowIcon"));
		pb_addapply = new QPushButton(icons_manager->loadIcon("AddUserButton"), tr("Add"), bottom, "add");
	}
	else
	{
		setCaption(tr("User info on %1").arg(User.altNick()));
		l_icon->setPixmap(icons_manager->loadIcon("ManageUsersWindowIcon"));
		pb_addapply = new QPushButton(icons_manager->loadIcon("UpdateUserButton"), tr("Update"), bottom, "update");
	}

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_addapply, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	createNotifier.notify(this);

	layoutHelper->addLabel(l_info);

	loadGeometry(this, "General", "ManageUsersDialogGeometry", 0, 30, 425, 500);
	kdebugf2();
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

	// UIN and disp
	QHBox *hb_uindisp = new QHBox(vgb_general);
	QVBox *vb_uin = new QVBox(hb_uindisp);
	QVBox *vb_disp = new QVBox(hb_uindisp);
	hb_uindisp->setSpacing(3);
	vb_uin->setSpacing(3);
	vb_disp->setSpacing(3);

	new QLabel(tr("Uin"), vb_uin);
	e_uin = new QLineEdit(vb_uin);
	e_uin->setMaxLength(8);
	e_uin->setValidator(new QIntValidator(1, 99999999, this));

	new QLabel(tr("AltNick"), vb_disp);
	e_altnick = new QLineEdit(vb_disp);
	// end UIN and disp

	// name and nick
	QHBox *hb_namenick = new QHBox(vgb_general);
	QVBox *vb_name = new QVBox(hb_namenick);
	QVBox *vb_nick = new QVBox(hb_namenick);
	hb_namenick->setSpacing(3);
	vb_name->setSpacing(3);
	vb_nick->setSpacing(3);

	new QLabel(tr("First name"), vb_name);
	e_firstname = new QLineEdit(vb_name);
	new QLabel(tr("Nickname"), vb_nick);
	e_nickname = new QLineEdit(vb_nick);
	// end name and nick

	// Surname & mobile
	QHBox *hb_surnamemobile = new QHBox(vgb_general);
	QVBox *vb_surname = new QVBox(hb_surnamemobile);
	QVBox *vb_mobile = new QVBox(hb_surnamemobile);
	hb_surnamemobile->setSpacing(3);
	vb_surname->setSpacing(3);
	vb_mobile->setSpacing(3);

	new QLabel(tr("Surname"), vb_surname);
	e_lastname = new QLineEdit(vb_surname);

	new QLabel(tr("Mobile"), vb_mobile);
	e_mobile = new QLineEdit(vb_mobile);
	// end Surname & mobile

	// Email
	QHBox *hb_email = new QHBox(vgb_general);
	QVBox *vb_email = new QVBox(hb_email);
	QVBox *vb_empty = new QVBox(hb_email, "space_for_advanced_userlist");
	hb_email->setSpacing(3);
	vb_email->setSpacing(3);
	vb_empty->setSpacing(3);

	new QLabel(tr("Email"), vb_email);
	e_email = new QLineEdit(vb_email);
	hb_email->setStretchFactor(vb_email, 1);
	hb_email->setStretchFactor(vb_empty, 1);
	// end Email

	QFrame *line1 = new QFrame(vgb_general);
	line1->setFrameShape(QFrame::HLine);
	line1->setFrameShadow(QFrame::Sunken);
	line1->setFrameShape(QFrame::HLine);

	// IP and DNS
	QHBox *hb_ipdns = new QHBox(vgb_general);
	QVBox *vb_ip = new QVBox(hb_ipdns);
	QVBox *vb_dns = new QVBox(hb_ipdns);
	hb_ipdns->setSpacing(3);
	vb_ip->setSpacing(3);
	vb_dns->setSpacing(3);

	new QLabel(tr("Address IP and Port"), vb_ip);
	e_addr = new QLineEdit(vb_ip);
	e_addr->setBackgroundMode(PaletteButton);

	new QLabel(tr("DNS name"), vb_dns);
	e_dnsname = new QLineEdit(vb_dns);
	
	e_dnsname->setBackgroundMode(PaletteButton);
	// end IP and DNS

	// Protocol Version and status
	QHBox *hb_protversionstate = new QHBox(vgb_general);
	QVBox *vb_protversion = new QVBox(hb_protversionstate);
	QVBox *vb_state = new QVBox(hb_protversionstate);
	hb_protversionstate->setSpacing(3);
	vb_protversion->setSpacing(3);
	vb_state->setSpacing(3);
	new QLabel(tr("Protocol version"), vb_protversion);
	e_ver = new QLineEdit(vb_protversion);
	e_ver->setBackgroundMode(PaletteButton);

	new QLabel(tr("Status"), vb_state);
	QLineEdit *e_status = new QLineEdit(vb_state);
	e_status->setBackgroundMode(PaletteButton);
	// end Protocol Version and status

	e_status->setReadOnly(true);
	e_addr->setReadOnly(true);
	e_ver->setReadOnly(true);
	e_dnsname->setReadOnly(true);

	e_nickname->setText(User.nickName());
	e_altnick->setText(User.altNick());
	e_firstname->setText(User.firstName());
	e_lastname->setText(User.lastName());
	e_mobile->setText(User.mobile());
	e_email->setText(User.email());

	if (User.usesProtocol("Gadu"))
	{
		if (gadu->currentStatus().isOffline())
			e_status->setText(tr("(Unknown)"));

		e_uin->setText(User.ID("Gadu"));

		if (User.hasIP("Gadu"))
		{
			e_addr->setText(User.IP("Gadu").toString());

			if (User.DNSName("Gadu").isEmpty())
			{
				dns = new QDns();
				dns->setLabel(User.IP("Gadu"));
				dns->setRecordType(QDns::Ptr);
				connect(dns, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
			}
			else
				e_dnsname->setText(User.DNSName("Gadu"));
		}
		else
		{
			e_addr->setText(tr("(Unknown)"));
			e_dnsname->setText(tr("(Unknown)"));
		}

		if (User.port("Gadu"))
			e_addr->setText(e_addr->text() + ':' + QString::number(User.port("Gadu")));
		else
			e_addr->setText(e_addr->text() + ':' + tr("(Unknown)"));

		unsigned int version = (User.protocolData("Gadu", "Version").toUInt() & 0x0000ffff);
		if (version)
		{
			QString s_temp;
			s_temp.sprintf("0x%02x", version);
			e_ver->setText(s_temp + " (" + versionToName(version) + ")");
		}
		else
			e_ver->setText(tr("(Unknown)"));

		e_status->setText(tr(User.status("Gadu").name().ascii()));
		QToolTip::add(e_status, User.status("Gadu").description());

		tw_main->setTabIconSet(vgb_general, User.status("Gadu").pixmap());
	}

	kdebugf2();
}

void UserInfo::setupTab2()
{
	kdebugf();

	scrollView = new QScrollView(tw_main);
	scrollView->setFrameStyle(QFrame::NoFrame);
	scrollView->setResizePolicy(QScrollView::AutoOneFit);
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::AlwaysOff);

	QVGroupBox *groupsTab = new QVGroupBox(scrollView->viewport());
	groupsTab->setFrameStyle(QFrame::NoFrame);
	scrollView->addChild(groupsTab);

	tw_main->addTab(scrollView, tr("Groups"));

	QStringList allGroups = groups_manager->groups();

	QStringList userGroups = User.data("Groups").toStringList();

	groupsBox = new QVBox(groupsTab);
	groupsBox->setSpacing(3);

	CONST_FOREACH(it, allGroups)
	{
		QHBox *box = new QHBox(groupsBox, *it);
		box->setSpacing(3);
		QCheckBox *checkBox=new QCheckBox(*it, box);
		checkBox->show();
		checkBox->setChecked(userGroups.contains(*it));

		QLabel *textLabel = new QLabel(box);
		textLabel->setText(tr("Icon:"));
		textLabel->setMaximumWidth(40);

		QLabel *pixmapLabel = new QLabel(box);
		QPixmap icon=icons_manager->loadIcon(config_file.readEntry("GroupIcon", *it, ""));
		pixmapLabel->setPixmap(icon.xForm(QWMatrix().scale((double)16/icon.width(), (double)16/icon.height())));
		pixmapLabel->setMaximumWidth(22);
		pixmapLabel->setMaximumHeight(22);
		pixmapLabels[*it] = pixmapLabel;

		QPushButton *changeIconButton = new QPushButton(box);
		changeIconButton->setPixmap(icons_manager->loadIcon("AddSelectPathDialogButton"));
		QToolTip::add(changeIconButton, tr("Change icon"));
		changeIconButton->setMaximumWidth(30);

		QPushButton *deleteIconButton = new QPushButton(box);
		deleteIconButton->setPixmap(icons_manager->loadIcon("RemoveSelectPathDialogButton"));
		QToolTip::add(deleteIconButton, tr("Delete icon"));
		deleteIconButton->setMaximumWidth(30);

		connect(changeIconButton, SIGNAL(clicked()), this, SLOT(selectIcon()));
		connect(deleteIconButton, SIGNAL(clicked()), this, SLOT(deleteIcon()));

		groups.append(checkBox);
	}

	newGroup = new QLineEdit(groupsTab);
	QPushButton *addNewGroup = new QPushButton(tr("Add new group"), groupsTab);
	connect(addNewGroup, SIGNAL(clicked()), this, SLOT(newGroupClicked()));
	connect(newGroup, SIGNAL(returnPressed()), this, SLOT(newGroupClicked()));

	kdebugf2();
}

bool UserInfo::acceptableGroupName(const QString &groupName)
{
	kdebugf();
	if (groupName.isEmpty())
	{
		kdebugf2();
		return false;
	}
	if (groupName.contains(","))
	{
		MessageBox::msg(tr("'%1' is prohibited").arg(','), true, "Warning");
		kdebugf2();
		return false;
	}
	if (groupName.contains(";"))
	{
		MessageBox::msg(tr("'%1' is prohibited").arg(';'), true, "Warning");
		kdebugf2();
		return false;
	}
	bool number;
	groupName.toLong(&number);
	if (number)
	{
		MessageBox::msg(tr("Numbers are prohibited"), true, "Warning");//because of gadu-gadu contact list format...
		kdebugf2();
		return false;
	}
	if (groupName == GroupsManager::tr("All") || groups_manager->groupExists(groupName))
	{
		MessageBox::msg(tr("This group already exists!"), true, "Warning");
		kdebugf2();
		return false;
	}
	kdebugf2();
	return true;
}

void UserInfo::newGroupClicked()
{
	kdebugf();
	QString groupName = newGroup->text();
	if (!acceptableGroupName(groupName))
	{
		kdebugf2();
		return;
	}
	CONST_FOREACH(checkbox, groups)
		if ((*checkbox)->text() == groupName)
		{
			MessageBox::msg(tr("This group already exists!"), true, "Warning", this);
			return;
		}

	QHBox *box = new QHBox(groupsBox, groupName);
	box->setSpacing(3);

	QCheckBox *checkBox = new QCheckBox(groupName, box);

	checkBox->setChecked(true);
	
	QLabel *textLabel = new QLabel(box);
	textLabel->setText(tr("Icon:"));
	textLabel->setMaximumWidth(40);
	
	QLabel *pixmapLabel = new QLabel(box);
	pixmapLabel->setMaximumWidth(22);
	pixmapLabel->setMaximumHeight(22);
	pixmapLabels[groupName] = pixmapLabel;

	QPushButton *changeIconButton = new QPushButton(box);
	changeIconButton->setPixmap(icons_manager->loadIcon("AddSelectPathDialogButton"));
	QToolTip::add(changeIconButton, tr("Change icon"));
	changeIconButton->setMaximumWidth(30);

	QPushButton *deleteIconButton = new QPushButton(box);
	deleteIconButton->setPixmap(icons_manager->loadIcon("CancelMessage"));
	QToolTip::add(deleteIconButton, tr("Delete icon"));
	deleteIconButton->setMaximumWidth(30);

	connect(changeIconButton, SIGNAL(clicked()), this, SLOT(selectIcon()));
	connect(deleteIconButton, SIGNAL(clicked()), this, SLOT(deleteIcon()));

	box->show();

	groups.append(checkBox);

	QTimer::singleShot(0, this, SLOT(scrollToBottom()));

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

	if (User.usesProtocol("Gadu"))
	{
		c_blocking->setChecked(User.protocolData("Gadu", "Blocking").toBool());
		c_offtouser->setChecked(User.protocolData("Gadu", "OfflineTo").toBool());
	}
	c_notify->setChecked(User.notify());
	// end Misc options

	kdebugf2();
}

UserInfo::~UserInfo()
{
	kdebugf();
	saveGeometry(this, "General", "ManageUsersDialogGeometry");
	delete dns;
	delete layoutHelper;
	kdebugf2();
}

void UserInfo::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void UserInfo::resultsReady()
{
	e_dnsname->setText(dns->hostNames()[0]);
}

void UserInfo::updateUserlist()
{
	kdebugf();

	QString id = QString::number(0);
	if (!e_uin->text().isEmpty())
		id = e_uin->text();

	if (e_altnick->text().isEmpty())
	{
		MessageBox::msg(tr("Altnick field cannot be empty."), false, "Warning", this);

		kdebugf2();
		return;
	}

	if (userlist->contains("Gadu", id)) // je¿eli istenieje ju¿ u¿ytkownik o danym ID...
	{
		UserListElement user = userlist->byID("Gadu", id);
		if (user != User) // ...i nie jest to ten, aktualnie edytowany...
		{
			if (user.isAnonymous()) // ...to je¶li istniej±cy kontakt jest anonimem...
			{
				userlist->removeUser(User);
				User = user; // ...to usuwamy edytowanego usera i zastêpujemy go anonimem
			}
			else
			{
				MessageBox::msg(tr("User known as %1 with that UIN is already in userlist.").arg(user.altNick()), false, "Warning", this);

				kdebugf2();
				return;
			}
		}
	}

	User.setFirstName(e_firstname->text());
	User.setLastName(e_lastname->text());
	User.setNickName(e_nickname->text());
	User.setAltNick(e_altnick->text());
	User.setMobile(e_mobile->text());

	if (User.usesProtocol("Gadu")) // there was an UIN so far?
	{
		if (User.ID("Gadu") != id) // uin was changed
		{
			User.deleteProtocol("Gadu");
			if (id.toUInt() != 0) // but it might be deleted
				User.addProtocol("Gadu", id);
		}
	}
	else // there was no UIN so far
		if (id.toUInt() != 0) // if it was filled, then we add new protocol
			User.addProtocol("Gadu", id);

	QStringList l;
	CONST_FOREACH(checkbox, groups)
		if ((*checkbox)->isChecked())
			l.append((*checkbox)->text());
	User.setData("Groups", l);

	User.setEmail(e_email->text());
	User.setNotify(c_notify->isChecked());
	if (User.usesProtocol("Gadu"))
	{
		User.setProtocolData("Gadu", "OfflineTo", c_offtouser->isChecked());
		User.setProtocolData("Gadu", "Blocking", c_blocking->isChecked());
	}
	User.setAnonymous(false);
	if (!userlist->contains(User))
		userlist->addUser(User);

	emit updateClicked(this);

	userlist->writeToConfig();
	xml_config_file->sync();

	close(true);

	kdebugf2();
}


void UserInfo::scrollToBottom()
{
	scrollView->setContentsPos(0, scrollView->contentsHeight());
}

void UserInfo::selectIcon()
{
	ImageDialog* iDialog = new ImageDialog(this);
	iDialog->setDir(config_file.readEntry("GroupIcon", "recentPath", "~/"));
	iDialog->setCaption(tr("Choose an icon"));
	iDialog->setFilter(tr("Icons (*.png *.xpm *.jpg)"));
	if (iDialog->exec() == QDialog::Accepted)
	{
		QString groupName = sender()->parent()->name();

		config_file.writeEntry("GroupIcon", "recentPath", iDialog->dirPath());
		config_file.writeEntry("GroupIcon", groupName, iDialog->selectedFile());

		groups_manager->setIconForTab(groupName);

		QPixmap icon = icons_manager->loadIcon(iDialog->selectedFile());
		pixmapLabels[groupName]->setPixmap(icon.xForm(QWMatrix().scale((double)16/icon.width(), (double)16/icon.height())));
	}
	delete iDialog;
}

void UserInfo::deleteIcon()
{
	QString groupName = sender()->parent()->name();

	config_file.removeVariable("GroupIcon", groupName);

	pixmapLabels[groupName]->setText("");

	groups_manager->setIconForTab(groupName);
}
