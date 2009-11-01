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
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"

#include "buddies/account-data/contact-account-data.h"
#include "gui/widgets/contact-account-data-widget.h"
#include "gui/windows/buddy-data-manager.h"
#include "gui/windows/message-box.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "buddy-data-window.h"

BuddyDataWindow::BuddyDataWindow(Buddy buddy, QWidget *parent) :
		QWidget(parent, Qt::Dialog), MyBuddy(buddy)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);

	createGui();

	setWindowTitle(tr("Merged Contact Properties - %1").arg(MyBuddy.display()));

	loadWindowGeometry(this, "General", "ManageUsersDialogGeometry", 0, 50, 425, 500);
}

BuddyDataWindow::~BuddyDataWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ManageUsersDialogGeometry");
	kdebugf2();
}

void BuddyDataWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	createTabs(layout);
	createButtons(layout);
}

void BuddyDataWindow::createTabs(QLayout *layout)
{
	QTabWidget *tabWidget = new QTabWidget(this);

	createGeneralTab(tabWidget);
	createGroupsTab(tabWidget);
	createPersonalInfoTab(tabWidget);
	createOptionsTab(tabWidget);
	layout->addWidget(tabWidget);
}

void BuddyDataWindow::createGeneralTab(QTabWidget *tabWidget)
{
	ContactTab = new ContactGeneralConfigurationWidget(MyBuddy, this);
	ContactTab->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	tabWidget->addTab(ContactTab, tr("General"));
}

void BuddyDataWindow::createGroupsTab(QTabWidget *tabWidget)
{
	GroupsTab = new ContactGroupsConfigurationWidget(MyBuddy, this);
	GroupsTab->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	tabWidget->addTab(GroupsTab, tr("Groups"));
}

void BuddyDataWindow::createPersonalInfoTab(QTabWidget *tabWidget)
{
	PersonalInfoTab = new ContactPersonalInfoConfigurationWidget(MyBuddy, this);
	PersonalInfoTab->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	tabWidget->addTab(PersonalInfoTab, tr("Personal Information"));
}

void BuddyDataWindow::createOptionsTab(QTabWidget *tabWidget)
{
	OptionsTab = new ContactOptionsConfigurationWidget(MyBuddy, this);
	OptionsTab->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	tabWidget->addTab(OptionsTab, tr("Options"));
}

void BuddyDataWindow::createButtons(QLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	QPushButton *applyButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Apply"), this);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *saveButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("OK"), this);
	buttons->addButton(saveButton, QDialogButtonBox::AcceptRole);

	connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddyAndClose()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddy()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);
}

void BuddyDataWindow::updateBuddy()
{
	BuddyManager::instance()->blockUpdatedSignal(MyBuddy);

	ContactTab->saveConfiguration();
	GroupsTab->saveConfiguration(); 
	OptionsTab->saveConfiguration(); 

	BuddyManager::instance()->unblockUpdatedSignal(MyBuddy);
}

void BuddyDataWindow::updateBuddyAndClose()
{
	updateBuddy();
	close();
}

void BuddyDataWindow::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}
