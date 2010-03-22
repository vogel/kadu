/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QIntValidator>
#include <QtGui/QRegExpValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHostInfo>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "groups_manager.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "userlist.h"

#include "userinfo.h"

CreateNotifier UserInfo::createNotifier;

UserInfo::UserInfo(UserListElement user, QWidget *parent)
	: QWidget(parent, Qt::Dialog), User(user),
	e_firstname(0), e_lastname(0), e_nickname(0), e_altnick(0), e_mobile(0), e_uin(0),
	e_addr(0), e_ver(0), e_email(0), e_dnsname(0), c_blocking(0), c_offtouser(0),
	c_notify(0), pb_addapply(0), tw_main(0), groups(), newGroup(0), groupsWidget(0), groupsLayout(0)
{
	kdebugf();

#ifdef Q_OS_WIN
	/* Workaround for Windows7 minimalization bug */
	setParent(NULL);
#endif

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowModality(Qt::WindowModal);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget;

	QLabel *l_icon = new QLabel;

	QVBoxLayout *left_layout = new QVBoxLayout(left);
	left_layout->addWidget(l_icon);
	left_layout->addStretch();

	QWidget *center = new QWidget();

	QLabel *l_info = new QLabel;
	l_info->setText(tr("This dialog box allows you to view and edit information about the selected contact."));
	l_info->setWordWrap(true);
#ifndef Q_OS_MAC
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	// end create main QLabel widgets (icon and app info)

	tw_main = new QTabWidget;

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(l_info);
	center_layout->addWidget(tw_main);

	// create our Tabs
	setupTab1();
	setupTab2();
	setupTab3();

	// create buttons and fill icon and app info
	QWidget *bottom = new QWidget;

	if (!userlist->contains(User, FalseForAnonymous))
	{
		setWindowTitle(tr("Add user"));
		l_icon->setPixmap(icons_manager->loadPixmap("AddUserWindowIcon"));
		pb_addapply = new QPushButton(icons_manager->loadIcon("AddUserButton"), tr("Add"));
	}
	else
	{
		setWindowTitle(tr("User info on %1").arg(User.altNick()));
		l_icon->setPixmap(icons_manager->loadPixmap("ManageUsersWindowIcon"));
		pb_addapply = new QPushButton(icons_manager->loadIcon("UpdateUserButton"), tr("Update"));
	}

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"));
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_addapply, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addStretch();
	bottom_layout->addWidget(pb_addapply);
	bottom_layout->addWidget(pb_close);

	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	createNotifier.notify(this);

	loadWindowGeometry(this, "General", "ManageUsersDialogGeometry", 0, 50, 425, 500);
	kdebugf2();
}

void UserInfo::setupTab1()
{
	kdebugf();

	QWidget *generalWidget = new QWidget(tw_main);

	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	scrollArea->setWidget(generalWidget);
	scrollArea->setWidgetResizable(true);

	tw_main->addTab(scrollArea, tr("General"));

	// UIN and disp
	e_uin = new QLineEdit(generalWidget);
	e_uin->setMaxLength(10);
	e_uin->setValidator(new QRegExpValidator(QRegExp("[1-9][0-9]*"), this));

	e_altnick = new QComboBox(generalWidget);
	e_altnick->setEditable(true);
	// end UIN and disp

	// name and nick
	e_firstname = new QLineEdit(generalWidget);
	e_nickname = new QLineEdit(generalWidget);
	// end name and nick

	// Surname & mobile
	e_lastname = new QLineEdit(generalWidget);
	e_mobile = new QLineEdit(generalWidget);
	// end Surname & mobile

	QLabel *emptyWidget = new QLabel("", generalWidget);
	emptyWidget->setName("space_for_advanced_userlist_label");

	QWidget *emptyWidget2 = new QWidget(generalWidget);
	emptyWidget2->setName("space_for_advanced_userlist_spinbox");

	// Email
	e_email = new QLineEdit(generalWidget);
	// end Email

	QFrame *line1 = new QFrame(generalWidget);
	line1->setFrameShape(QFrame::HLine);
	line1->setFrameShadow(QFrame::Sunken);
	line1->setFrameShape(QFrame::HLine);

	// IP and DNS
	e_addr = new QLineEdit(generalWidget);
	e_addr->setBackgroundMode(Qt::PaletteButton);

	e_dnsname = new QLineEdit(generalWidget);
	e_dnsname->setBackgroundMode(Qt::PaletteButton);
	// end IP and DNS

	// Protocol Version and status
	e_ver = new QLineEdit(generalWidget);
	e_ver->setBackgroundMode(Qt::PaletteButton);

	QLineEdit *e_status = new QLineEdit(generalWidget);
	e_status->setBackgroundMode(Qt::PaletteButton);
	// end Protocol Version and status

	QGridLayout * generalLayout = new QGridLayout(generalWidget);
	generalLayout->setSpacing(3);
	generalLayout->addWidget(new QLabel(tr("Uin"), generalWidget), 0, 0);
	generalLayout->addWidget(e_uin, 1, 0);
	generalLayout->addWidget(new QLabel(tr("AltNick"), generalWidget), 0, 1);
	generalLayout->addWidget(e_altnick, 1, 1);
	generalLayout->addWidget(new QLabel(tr("First name"), generalWidget), 2, 0);
	generalLayout->addWidget(e_firstname, 3, 0);
	generalLayout->addWidget(new QLabel(tr("Nickname"), generalWidget), 2, 1);
	generalLayout->addWidget(e_nickname, 3, 1);
	generalLayout->addWidget(new QLabel(tr("Surname"), generalWidget), 4, 0);
	generalLayout->addWidget(e_lastname, 5, 0);
	generalLayout->addWidget(new QLabel(tr("Mobile"), generalWidget), 4, 1);
	generalLayout->addWidget(e_mobile, 5, 1);
	generalLayout->addWidget(new QLabel(tr("Email"), generalWidget), 6, 0);
	generalLayout->addWidget(e_email, 7, 0);
	generalLayout->addWidget(emptyWidget, 6, 1);
	generalLayout->addWidget(emptyWidget2, 7, 1);
	generalLayout->addWidget(line1, 8, 0, 1, 2);
	generalLayout->addWidget(new QLabel(tr("Address IP and Port"), generalWidget), 9, 0);
	generalLayout->addWidget(e_addr, 10, 0);
	generalLayout->addWidget(new QLabel(tr("DNS name"), generalWidget), 9, 1);
	generalLayout->addWidget(e_dnsname, 10, 1);
	generalLayout->addWidget(new QLabel(tr("Protocol version"), generalWidget), 11, 0);
	generalLayout->addWidget(e_ver, 12, 0);
	generalLayout->addWidget(new QLabel(tr("Status"), generalWidget), 11, 1);
	generalLayout->addWidget(e_status, 12, 1);

	e_status->setReadOnly(true);
	e_addr->setReadOnly(true);
	e_ver->setReadOnly(true);
	e_dnsname->setReadOnly(true);

	e_nickname->setText(User.nickName());
	e_firstname->setText(User.firstName());
	e_lastname->setText(User.lastName());
	e_mobile->setText(User.mobile());
	e_email->setText(User.email());

	connect(e_nickname, SIGNAL(editingFinished()), this, SLOT(updateAltNick()));
	connect(e_firstname, SIGNAL(editingFinished()), this, SLOT(updateAltNick()));
	connect(e_lastname, SIGNAL(editingFinished()), this, SLOT(updateAltNick()));

	e_altnick->setEditText(User.altNick());
	updateAltNick();

	if (User.usesProtocol("Gadu"))
	{
		if (gadu->currentStatus().isOffline())
			e_status->setText(tr("(Unknown)"));

		e_uin->setText(User.ID("Gadu"));

		if (User.hasIP("Gadu"))
		{
			e_addr->setText(User.IP("Gadu").toString());

			if (User.DNSName("Gadu").isEmpty())
				QHostInfo::lookupHost(User.IP("Gadu").toString(), this, SLOT(resultsReady(QHostInfo)));
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

		e_status->setText(UserStatus::tr(User.status("Gadu").name()));
		e_status->setToolTip(User.status("Gadu").description());

		tw_main->setTabIconSet(generalWidget, User.status("Gadu").pixmap());
	}

	kdebugf2();
}

void UserInfo::updateAltNick()
{
	QStringList list;

	if (!e_altnick->currentText().isEmpty())
		list << e_altnick->currentText();

	if (!e_nickname->text().isEmpty() && !list.contains(e_nickname->text()))
		list << e_nickname->text();

	if (!e_firstname->text().isEmpty())
	{
		if (!list.contains(e_firstname->text()))
			list << e_firstname->text();

		if (!e_lastname->text().isEmpty())
		{
			list << QString("%1 %2").arg(e_firstname->text(), e_lastname->text());
			list << QString("%2 %1").arg(e_firstname->text(), e_lastname->text());

			if (!e_nickname->text().isEmpty())
			{
				list << QString("%1 \"%2\" %3").arg(e_firstname->text(), e_nickname->text(), e_lastname->text());
				list << QString("%3 \"%2\" %1").arg(e_firstname->text(), e_nickname->text(), e_lastname->text());
				list << QString("%1 %2 (%3)").arg(e_lastname->text(), e_firstname->text(), e_nickname->text());
				list << QString("%2 %1 (%3)").arg(e_lastname->text(), e_firstname->text(), e_nickname->text());
			}
		}
	}

	e_altnick->clear();
	e_altnick->addItems(list);
}

void UserInfo::setupTab2()
{
	kdebugf();

	scrollArea = new QScrollArea(tw_main);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *groupsTab = new QWidget(tw_main);
	QVBoxLayout *groupsTabLayout = new QVBoxLayout(groupsTab);

	tw_main->addTab(scrollArea, tr("Groups"));

	scrollArea->setWidget(groupsTab);
	scrollArea->setWidgetResizable(true);

	QStringList allGroups = groups_manager->groups();

	QStringList userGroups = User.data("Groups").toStringList();

	groupsWidget = new QWidget(groupsTab);
	groupsLayout = new QVBoxLayout(groupsWidget);
	groupsLayout->setSpacing(3);

	foreach(const QString &it, allGroups)
	{
		QWidget *box = new QWidget(groupsWidget);
		QHBoxLayout *boxLayout = new QHBoxLayout(box);
		boxLayout->setSpacing(3);
		QCheckBox *checkBox = new QCheckBox(it, box);
		checkBox->setChecked(userGroups.contains(it));

		QLabel *textLabel = new QLabel(box);
		textLabel->setText(tr("Icon:"));
		textLabel->setMaximumWidth(40);

		QLabel *pixmapLabel = new QLabel(box);
		QPixmap icon = icons_manager->loadPixmap(config_file.readEntry("GroupIcon", it, ""));
		pixmapLabel->setPixmap(icon.xForm(QWMatrix().scale((double)16/icon.width(), (double)16/icon.height())));
		pixmapLabel->setMaximumWidth(22);
		pixmapLabel->setMaximumHeight(22);
		pixmapLabels[it] = pixmapLabel;

		QPushButton *changeIconButton = new QPushButton(box);
		changeIconButton->setPixmap(icons_manager->loadPixmap("AddSelectPathDialogButton"));
		changeIconButton->setToolTip(tr("Change icon"));
		changeIconButton->setMaximumWidth(30);

		QPushButton *deleteIconButton = new QPushButton(box);
		deleteIconButton->setPixmap(icons_manager->loadPixmap("RemoveSelectPathDialogButton"));
		deleteIconButton->setToolTip(tr("Delete icon"));
		deleteIconButton->setMaximumWidth(30);

		boxLayout->addWidget(checkBox);
		boxLayout->addWidget(textLabel);
		boxLayout->addWidget(pixmapLabel);
		boxLayout->addWidget(changeIconButton);
		boxLayout->addWidget(deleteIconButton);

		groupsLayout->addWidget(box);

		connect(changeIconButton, SIGNAL(clicked()), this, SLOT(selectIcon()));
		connect(deleteIconButton, SIGNAL(clicked()), this, SLOT(deleteIcon()));

		groups.append(checkBox);
	}

	newGroup = new QLineEdit(groupsTab);
	QPushButton *addNewGroup = new QPushButton(tr("Add new group"), groupsTab);

	groupsTabLayout->addWidget(groupsWidget);
	groupsTabLayout->addWidget(newGroup);
	groupsTabLayout->addWidget(addNewGroup);

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
	foreach(QCheckBox *checkbox, groups)
		if (checkbox->text() == groupName)
		{
			MessageBox::msg(tr("This group already exists!"), true, "Warning", this);
			return;
		}

	QWidget *box = new QWidget(groupsWidget);
	QHBoxLayout *boxLayout = new QHBoxLayout(box);
	boxLayout->setSpacing(3);

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
	changeIconButton->setPixmap(icons_manager->loadPixmap("AddSelectPathDialogButton"));
	changeIconButton->setToolTip(tr("Change icon"));
	changeIconButton->setMaximumWidth(30);

	QPushButton *deleteIconButton = new QPushButton(box);
	deleteIconButton->setPixmap(icons_manager->loadPixmap("RemoveSelectPathDialogButton"));
	deleteIconButton->setToolTip(tr("Delete icon"));
	deleteIconButton->setMaximumWidth(30);

	boxLayout->addWidget(checkBox);
	boxLayout->addWidget(textLabel);
	boxLayout->addWidget(pixmapLabel);
	boxLayout->addWidget(changeIconButton);
	boxLayout->addWidget(deleteIconButton);

	groupsLayout->addWidget(box);

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
	QWidget *othersWidget = new QWidget(tw_main);

	othersWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

	QVBoxLayout *othersLayout = new QVBoxLayout(othersWidget);

	c_blocking = new QCheckBox(tr("Block user"), othersWidget);
	c_offtouser = new QCheckBox(tr("Offline to user"), othersWidget);
	c_notify = new QCheckBox(tr("Notify about status changes"), othersWidget);

	if (!config_file.readBoolEntry("General", "PrivateStatus"))
		c_offtouser->setEnabled(false);

	if (User.usesProtocol("Gadu"))
	{
		c_blocking->setChecked(User.protocolData("Gadu", "Blocking").toBool());
		c_offtouser->setChecked(User.protocolData("Gadu", "OfflineTo").toBool());
	}
	c_notify->setChecked(User.notify());

	othersLayout->addWidget(c_blocking);
	othersLayout->addWidget(c_offtouser);
	othersLayout->addWidget(c_notify);
	othersLayout->addStretch();

	tw_main->addTab(othersWidget, tr("Others"));
	// end Misc options

	kdebugf2();
}

UserInfo::~UserInfo()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ManageUsersDialogGeometry");
	kdebugf2();
}

void UserInfo::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void UserInfo::resultsReady(const QHostInfo &host)
{
    	if (host.error() == QHostInfo::NoError)
		e_dnsname->setText(host.hostName());
}

void UserInfo::updateUserlist()
{
	kdebugf();

	QString id = QString::number(0);
	if (!e_uin->text().isEmpty())
		id = e_uin->text();

	if (e_altnick->currentText().isEmpty())
	{
		MessageBox::msg(tr("Altnick field cannot be empty."), false, "Warning", this);

		kdebugf2();
		return;
	}

	if (userlist->contains("Gadu", id)) // je�eli istenieje ju� u�ytkownik o danym ID...
	{
		UserListElement user = userlist->byID("Gadu", id);
		if (user != User) // ...i nie jest to ten, aktualnie edytowany...
		{
			if (user.isAnonymous()) // ...to je�li istniej�cy kontakt jest anonimem...
			{
				userlist->removeUser(User);
				User = user; // ...to usuwamy edytowanego usera i zast�pujemy go anonimem
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
	User.setAltNick(e_altnick->currentText());
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
	foreach(QCheckBox *checkbox, groups)
		if (checkbox->isChecked())
			l.append(checkbox->text());
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
	scrollArea->verticalScrollBar()->setValue(scrollArea->widget()->height());
}

void UserInfo::selectIcon()
{
	ImageDialog* iDialog = new ImageDialog(this);
	iDialog->setDir(config_file.readEntry("GroupIcon", "recentPath", "~/"));
	iDialog->setCaption(tr("Choose an icon"));
	iDialog->setFilter(tr("Icons (*.png *.xpm *.jpg)"));
	if (iDialog->exec() == QDialog::Accepted)
	{
		QString groupName;
		const QCheckBox *checkBox = 0;
		
		foreach(QObject *child, sender()->parent()->children())
		{
			checkBox = dynamic_cast<const QCheckBox*>(child);
			
			if(checkBox)
			{
				groupName = checkBox->text();
				break;
			}
		}

		config_file.writeEntry("GroupIcon", "recentPath", iDialog->dirPath());
		config_file.writeEntry("GroupIcon", groupName, iDialog->selectedFile());

		groups_manager->setIconForTab(groupName);

		pixmapLabels[groupName]->setPixmap(icons_manager->loadPixmap(iDialog->selectedFile()).scaled(QSize(16,16)));
	}
	delete iDialog;
}

void UserInfo::deleteIcon()
{
	QString groupName;
	const QCheckBox *checkBox = 0;
	
	foreach(QObject *child, sender()->parent()->children())
	{
		checkBox = dynamic_cast<const QCheckBox*>(child);
		
		if (checkBox)
		{
			groupName = checkBox->text();
			break;
		}
	}

	config_file.removeVariable("GroupIcon", groupName);

	pixmapLabels[groupName]->setPixmap(QPixmap());

	groups_manager->setIconForTab(groupName);
}
