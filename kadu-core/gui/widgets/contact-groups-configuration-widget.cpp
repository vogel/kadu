/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QScrollArea>

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"
#include "misc/misc.h"

#include "contact-groups-configuration-widget.h"

ContactGroupsConfigurationWidget::ContactGroupsConfigurationWidget(QWidget *parent)
	: QScrollArea(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactGroupsConfigurationWidget::~ContactGroupsConfigurationWidget()
{
}

void ContactGroupsConfigurationWidget::createGui()
{
// QVBoxLayout *mainLayout = new QVBoxLayout(this);
	/*QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->*/setFrameStyle(QFrame::NoFrame);
	/*scrollArea->*/setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	/*scrollArea->*/setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//scrollArea->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	QWidget *groupsTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(groupsTab);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnMinimumWidth(5, 100);
	layout->setColumnMinimumWidth(6, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;

	/*scrollArea->*/setWidget(groupsTab);
	/*scrollArea->*/setWidgetResizable(true);

	QLabel *tabLabel = new QLabel(tr("Groups"), this);
	layout->addWidget(tabLabel, 0, 0, 1, 3);

	QLabel *tabSubLabel = new QLabel(tr("Add %s to the groups below by checking the box next to the appropriate groups.").arg("Marian"), this);
	layout->addWidget(tabSubLabel, 1, 1, 1, 4);

	for (int c = 0; c < 3; c++)
	{
		QGroupBox *accountGroupsBox = new QGroupBox(tr("Gadu-Gadu"));
		QGridLayout *accountGroupsLayout = new QGridLayout(accountGroupsBox);
		//communicationLayout->setColumnMinimumWidth(1, 100);
		//accountGroupsLayout->setColumnStretch(0, 2);
		accountGroupsLayout->setColumnStretch(1, 5);

		row = 0;

		for (int a = 0; a <= 10; a++)
		{
			QCheckBox *groupCheckBox = new QCheckBox(tr("Group name"), this);
			//QLabel *groupLabel = new QLabel(tr("Group name"));
			accountGroupsLayout->addWidget(groupCheckBox, row++, 0, 1, 2);
			//accountGroupsLayout->addWidget(groupLabel, row++, 1, 1, 3);
		}

		layout->addWidget(accountGroupsBox, c+2, 1, 1, 6);
	}

// 	QWidget *blank = new QWidget(this);
// 	blank->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
// 	layout->addWidget(blank, 2, 1, 1, 4);

// 	QVBoxLayout *groupsLayout = new QVBoxLayout(this);
// 	groupsLayout->setSpacing(3);
// 
// 	//foreach(Group* group , GroupManager::instance()->groups())
// 	for (int i = 1; i < 10; i++)
// 	{
// 		QCheckBox *checkBox = new QCheckBox("Group name "+i /**group->name()*/);
// 		///checkBox->setChecked(CurrentContact.isInGroup(group));
// 
// 		groupsLayout->addWidget(checkBox);
// 
// 		///groups.append(checkBox);
// 	}
// 
// 	QLineEdit *newGroup = new QLineEdit(this);
// 	QPushButton *addNewGroup = new QPushButton(tr("Add new group"), this);
// 
// 	//groupsLayout->addWidget(groupsWidget);
// 	groupsLayout->addWidget(newGroup);
// 	groupsLayout->addWidget(addNewGroup);
// 
// 	connect(addNewGroup, SIGNAL(clicked()), this, SLOT(newGroupClicked()));
// 	connect(newGroup, SIGNAL(returnPressed()), this, SLOT(newGroupClicked()));
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
// 	groupsLayout = new QVBoxLayout(groupsWidget);
// 	groupsLayout->setSpacing(3);
// 
// 	foreach(Group* group , GroupManager::instance()->groups())
// 	{
// 		QCheckBox *checkBox = new QCheckBox(group->name());
// 		checkBox->setChecked(CurrentContact.isInGroup(group));
// 
// 		groupsLayout->addWidget(checkBox);
// 
// 		groups.append(checkBox);
// 	}
// 
// 	newGroup = new QLineEdit(groupsTab);
// 	QPushButton *addNewGroup = new QPushButton(tr("Add new group"), groupsTab);
// 
// 	groupsTabLayout->addWidget(groupsWidget);
// 	groupsTabLayout->addWidget(newGroup);
// 	groupsTabLayout->addWidget(addNewGroup);
// 
// 	connect(addNewGroup, SIGNAL(clicked()), this, SLOT(newGroupClicked()));
// 	connect(newGroup, SIGNAL(returnPressed()), this, SLOT(newGroupClicked()));

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
