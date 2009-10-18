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
#include <QtGui/QDialogButtonBox>
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
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "gui/widgets/contact-account-data-widget.h"
#include "gui/windows/contact-data-manager.h"
#include "gui/windows/message-box.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "contact-data-window.h"

ContactDataWindow::ContactDataWindow(Contact contact, QWidget *parent) :
		QWidget(parent, Qt::Dialog), CurrentContact(contact)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	// po co? setWindowModality(Qt::WindowModal);

	createGui();

	setWindowTitle(CurrentContact.isAnonymous()
			? tr("Add user")
			: tr("User info on %1").arg(CurrentContact.display()));

	loadWindowGeometry(this, "General", "ManageUsersDialogGeometry", 0, 50, 425, 500);
}

ContactDataWindow::~ContactDataWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ManageUsersDialogGeometry");
	kdebugf2();
}

void ContactDataWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	createTabs(layout);
	createButtons(layout);
}

void ContactDataWindow::createTabs(QLayout *layout)
{
	QTabWidget *tabWidget = new QTabWidget(this);

	createContactTab(tabWidget);
	createGroupsTab(tabWidget);
	createAccountsTabs(tabWidget);
	layout->addWidget(tabWidget);
}

void ContactDataWindow::createContactTab(QTabWidget *tabWidget)
{
	ConfigurationWidget *contactConfiguration = new ConfigurationWidget(new ContactDataManager(CurrentContact, this), tabWidget);
	contactConfiguration->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	contactConfiguration->appendUiFile(dataPath("kadu/configuration/contact-data.ui"));

	ConfigurationWidgets.append(contactConfiguration);

	tabWidget->addTab(contactConfiguration, tr("General"));
}

void ContactDataWindow::createGroupsTab(QTabWidget *tabWidget)
{
}

void ContactDataWindow::createAccountsTabs(QTabWidget *tabWidget)
{
	foreach (Account *account, CurrentContact.accounts())
		createAccountTab(account, tabWidget);
}

void ContactDataWindow::createAccountTab(Account *account, QTabWidget *tabWidget)
{
	if (!account || !account->protocol())
		return;

	ProtocolFactory *protocolFactory = account->protocol()->protocolFactory();
	ContactAccountData *contactAccountData = CurrentContact.accountData(account);

	if (!contactAccountData || !protocolFactory)
		return;

	ContactAccountDataWidget *contactAccountDataWidget = protocolFactory->newContactAccountDataWidget(contactAccountData, this);
	if (!contactAccountDataWidget)
		return;

	contactAccountDataWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	contactAccountDataWidget->loadConfiguration();

	ConfigurationWidgets.append(contactAccountDataWidget);
	tabWidget->addTab(contactAccountDataWidget, account->name());
}

void ContactDataWindow::createButtons(QLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(IconsManager::instance()->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(updateContactAndClose()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(updateContact()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);
}

void ContactDataWindow::updateContact()
{
	ContactManager::instance()->blockUpdatedSignal(CurrentContact);

	foreach (ConfigurationWidget *configurationWidget, ConfigurationWidgets)
		configurationWidget->saveConfiguration();

	ContactManager::instance()->unblockUpdatedSignal(CurrentContact);
}

void ContactDataWindow::updateContactAndClose()
{
	updateContact();
	close();
}

void ContactDataWindow::updateDisplay()
{
// 	QStringList list;
// 	if (!e_display->currentText().isEmpty())
// 		list << e_display->currentText();
// 	if (!e_nickname->text().isEmpty() && !list.contains(e_nickname->text()))
// 		list << e_nickname->text();
// 	if (!e_firstname->text().isEmpty())
// 	{
// 		if (!list.contains(e_firstname->text()))
// 			list << e_firstname->text();
// 		if (!e_lastname->text().isEmpty())
// 		{
// 			list << e_firstname->text() + " " + e_lastname->text();
// 			list << e_lastname->text() + " " + e_firstname->text();
// 		}
// 	}
// 	e_display->clear();
// 	e_display->addItems(list);
}
/*
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
		checkBox->setChecked(CurrentContact.isInGroup(group));

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
	GroupManager::instance()->byName(groupName);

	kdebugf2();
}*/

void ContactDataWindow::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}
