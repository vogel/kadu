/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QCheckBox>
#include <QtGui/QIntValidator>
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

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"

#include "gui/widgets/contact-account-data-widget.h"

#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "xml_config_file.h"

#include "contact-data-window.h"

CreateNotifier ContactDataWindow::createNotifier;

ContactDataWindow::ContactDataWindow(Contact user, QWidget *parent)
	: QWidget(parent, Qt::Dialog), User(user),
	e_firstname(0), e_lastname(0), e_nickname(0), e_display(0), e_mobile(0), e_id(0),
	e_addr(0), e_ver(0), e_email(0), e_dnsname(0), c_blocking(0), c_offtouser(0),
	c_notify(0), pb_addapply(0), tw_main(0), groups(), newGroup(0), groupsWidget(0), groupsLayout(0)
{
	kdebugf();

	UserAccount = User.prefferedAccount();

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

	if (User.isAnonymous())
	{
		setWindowTitle(tr("Add user"));
		l_icon->setPixmap(icons_manager->loadPixmap("AddUserWindowIcon"));
		pb_addapply = new QPushButton(icons_manager->loadIcon("AddUserButton"), tr("Add"));
	}
	else
	{
		setWindowTitle(tr("User info on %1").arg(User.display()));
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

void ContactDataWindow::setupTab1()
{
	kdebugf();

	QWidget *generalWidget = new QWidget(tw_main);

	tw_main->addTab(generalWidget, tr("General"));

	// TODO: 0.6.6 we need it if we use only one account

	// ID and disp
	//e_id = new QLineEdit(generalWidget);
	//e_id->setMaxLength(8);
	// TODO: validateId from protocol
	//e_id->setValidator(new QIntValidator(1, 99999999, this));

	e_display = new QComboBox(generalWidget);
	e_display->setEditable(true);
	// end ID and disp

	// name and nick
	e_firstname = new QLineEdit(generalWidget);
	e_nickname = new QLineEdit(generalWidget);
	// end name and nick

	// Surname & mobile
	e_lastname = new QLineEdit(generalWidget);
	e_mobile = new QLineEdit(generalWidget);
	// end Surname & mobile

	QLabel *emptyWidget = new QLabel("", generalWidget);
	emptyWidget->setObjectName("space_for_advanced_userlist_label");

	QWidget *emptyWidget2 = new QWidget(generalWidget);
	emptyWidget2->setObjectName("space_for_advanced_userlist_spinbox");

	// Email
	e_email = new QLineEdit(generalWidget);
	// end Email

	//QFrame *line1 = new QFrame(generalWidget);
	//line1->setFrameShape(QFrame::HLine);
	//line1->setFrameShadow(QFrame::Sunken);
	//line1->setFrameShape(QFrame::HLine);

	// IP and DNS
	//e_addr = new QLineEdit(generalWidget);
	//e_addr->setBackgroundMode(Qt::PaletteButton);

	//e_dnsname = new QLineEdit(generalWidget);
	//e_dnsname->setBackgroundMode(Qt::PaletteButton);
	// end IP and DNS

	// Protocol Version and status
	//e_ver = new QLineEdit(generalWidget);
	//e_ver->setBackgroundMode(Qt::PaletteButton);

	//QLineEdit *e_status = new QLineEdit(generalWidget);
	//e_status->setBackgroundMode(Qt::PaletteButton);
	// end Protocol Version and status

	QGridLayout * generalLayout = new QGridLayout(generalWidget);
	generalLayout->setSpacing(3);
	generalLayout->addWidget(new QLabel(tr("Display"), generalWidget), 0, 0);
	generalLayout->addWidget(e_display, 1, 0);
	generalLayout->addWidget(new QLabel(tr("Nickname"), generalWidget), 0, 1);
	generalLayout->addWidget(e_nickname, 1, 1);
	generalLayout->addWidget(new QLabel(tr("First name"), generalWidget), 2, 0);
	generalLayout->addWidget(e_firstname, 3, 0);
	generalLayout->addWidget(new QLabel(tr("Surname"), generalWidget), 2, 1);
	generalLayout->addWidget(e_lastname, 3, 1);
	generalLayout->addWidget(new QLabel(tr("Email"), generalWidget), 4, 0);
	generalLayout->addWidget(e_email, 5, 0);
	generalLayout->addWidget(new QLabel(tr("Mobile"), generalWidget), 4, 1);
	generalLayout->addWidget(e_mobile, 5, 1);
	//generalLayout->addWidget(new QLabel(tr("Email"), generalWidget), 6, 0);
	//generalLayout->addWidget(e_email, 7, 0);
	generalLayout->addWidget(emptyWidget, 6, 1);
	generalLayout->addWidget(emptyWidget2, 7, 1);
	//generalLayout->addWidget(line1, 8, 0, 1, 2);
	//generalLayout->addWidget(new QLabel(tr("Address IP and Port"), generalWidget), 9, 0);
	//generalLayout->addWidget(e_addr, 10, 0);
	//generalLayout->addWidget(new QLabel(tr("DNS name"), generalWidget), 9, 1);
	//generalLayout->addWidget(e_dnsname, 10, 1);
	//generalLayout->addWidget(new QLabel(tr("Protocol version"), generalWidget), 11, 0);
	//generalLayout->addWidget(e_ver, 12, 0);
	//generalLayout->addWidget(new QLabel(tr("Status"), generalWidget), 11, 1);
	//generalLayout->addWidget(e_status, 12, 1);

	//e_status->setReadOnly(true);
	//e_addr->setReadOnly(true);
	//e_ver->setReadOnly(true);
	//e_dnsname->setReadOnly(true);

	e_nickname->setText(User.nickName());
	e_firstname->setText(User.firstName());
	e_lastname->setText(User.lastName());
	e_mobile->setText(User.mobile());
	e_email->setText(User.email());

	//Protocol *userProtocol = UserAccount->protocol();

	connect(e_nickname, SIGNAL(editingFinished()), this, SLOT(updateDisplay()));
	connect(e_firstname, SIGNAL(editingFinished()), this, SLOT(updateDisplay()));
	connect(e_lastname, SIGNAL(editingFinished()), this, SLOT(updateDisplay()));

	e_display->setEditText(User.display());
	updateDisplay();
	// TODO move to gadu.ui and add as contact account data widget ...
	/*if (User.usesProtocol("Gadu"))
	{
		if (!userProtocol->isConnected())
			e_status->setText(tr("(Unknown)"));

		e_id->setText(User.id(UserAccount));

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

		ContactAccountData *contactData = User.accountData(UserAccount);
		Status status = contactData->status();

// TODO: 0.6.6
		e_status->setText(QString::number(status.type()));
		e_status->setToolTip(status.description());

		tw_main->setTabIconSet(generalWidget, account->statusPixmap(status));
	}*/

	kdebugf2();
}

void ContactDataWindow::updateDisplay()
{
	QStringList list;
	if (!e_display->currentText().isEmpty())
		list << e_display->currentText();
	if (!e_nickname->text().isEmpty() && !list.contains(e_nickname->text()))
		list << e_nickname->text();
	if (!e_firstname->text().isEmpty())
	{
		if (!list.contains(e_firstname->text()))
			list << e_firstname->text();
		if (!e_lastname->text().isEmpty())
		{
			list << e_firstname->text() + " " + e_lastname->text();
			list << e_lastname->text() + " " + e_firstname->text();
		}
	}
	e_display->clear();
	e_display->addItems(list);
}

void ContactDataWindow::setupTab2()
{
	kdebugf();

	// TODO: 0.6.6 move to ContactGroupsConfigurationWidget

	scrollArea = new QScrollArea(tw_main);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *groupsTab = new QWidget(tw_main);
	QVBoxLayout *groupsTabLayout = new QVBoxLayout(groupsTab);

	tw_main->addTab(scrollArea, tr("Groups"));

	scrollArea->setWidget(groupsTab);
	scrollArea->setWidgetResizable(true);

	groupsWidget = new QWidget(groupsTab);
	groupsLayout = new QVBoxLayout(groupsWidget);
	groupsLayout->setSpacing(3);

	foreach(Group* group , GroupManager::instance()->groups())
	{
		QCheckBox *checkBox = new QCheckBox(group->name());
		checkBox->setChecked(User.isInGroup(group));

		groupsLayout->addWidget(checkBox);

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

void ContactDataWindow::newGroupClicked()
{
	kdebugf();
	QString groupName = newGroup->text();
	if (!GroupManager::instance()->acceptableGroupName(groupName))
	{
		kdebugf2();
		return;
	}

	QCheckBox *checkBox = new QCheckBox(groupName);

	checkBox->setChecked(true);

	groupsLayout->addWidget(checkBox);

	checkBox->show();

	groups.append(checkBox);

	QTimer::singleShot(0, this, SLOT(scrollToBottom()));

	//create new group
	GroupManager::instance()->byName(groupName, true);

	kdebugf2();
}


void ContactDataWindow::setupTab3()
{
	kdebugf();

	foreach (Account * account, User.accounts())
	{
		if (!account || !account->protocol())
			continue;

		ProtocolFactory *protocolFactory = account->protocol()->protocolFactory();

		if (!User.hasAccountData(account) || !protocolFactory)
			continue;

		ContactAccountData* contactAccountData = User.accountData(account);

		ContactAccountDataWidget* contactAccountDataWidget = protocolFactory-> newContactAccountDataWidget(contactAccountData, tw_main);

		if (!contactAccountDataWidget)
			continue;

		contactAccountDataWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
		contactAccountDataWidget->loadConfiguration();

		tw_main->addTab(contactAccountDataWidget, account->name());

		dataWidgets.append(contactAccountDataWidget);
	}

	kdebugf2();
}

ContactDataWindow::~ContactDataWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ManageUsersDialogGeometry");
	kdebugf2();
}

void ContactDataWindow::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void ContactDataWindow::resultsReady(const QHostInfo &host)
{
    	if (host.error() == QHostInfo::NoError)
		e_dnsname->setText(host.hostName());
}

void ContactDataWindow::updateUserlist()
{
	kdebugf();

	foreach (ContactAccountDataWidget* widget, widgets())
		widget->saveConfiguration();
//TODO
	foreach(QCheckBox *checkbox, groups)
		if (checkbox->isChecked())
		{
			Group *group = GroupManager::instance()->byName(checkbox->text(), false);
			User.addToGroup(group);
		}
/*
	QString id = QString::number(0);
	if (!e_id->text().isEmpty())
		id = e_id->text();

	if (e_id->currentText().isEmpty())
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
	User.setDisplay(e_display->currentText());
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

// TODO: 0.6.6
// 	userlist->writeToConfig();
	xml_config_file->sync();
*/
	close();

	kdebugf2();
}


void ContactDataWindow::scrollToBottom()
{
	scrollArea->verticalScrollBar()->setValue(scrollArea->widget()->height());
}

