/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include "configuration/configuration-contact-account-data-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"
#include "misc/misc.h"

#include "contact-groups-configuration-widget.h"

ContactGroupsConfigurationWidget::ContactGroupsConfigurationWidget(Contact &contact, QWidget *parent)
	: QScrollArea(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactGroupsConfigurationWidget::~ContactGroupsConfigurationWidget()
{
}

void ContactGroupsConfigurationWidget::createGui()
{
	setFrameStyle(QFrame::NoFrame);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *groupsTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(groupsTab);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(5, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;
	
	layout->setRowStretch(row++, 1); 

	setWidget(groupsTab);
	setWidgetResizable(true);

	QLabel *tabSubLabel = new QLabel(tr("Add %1 to the groups below by checking the box next to the appropriate groups.").arg(CurrentContact.display()), this);
	layout->setRowStretch(row, 1);
	layout->addWidget(tabSubLabel, row++, 2, 1, 4);

	layout->setRowStretch(row++, 3);

	foreach (Group *group, GroupManager::instance()->groups())
	{
		QCheckBox *groupCheckBox = new QCheckBox(group->name(), this);
		groupCheckBox->setChecked(CurrentContact.isInGroup(group));
		layout->addWidget(groupCheckBox, row++, 2, 1, 2);
		GroupCheckBoxList.append(groupCheckBox);
	}

	layout->setRowStretch(row, 100);
}

void ContactGroupsConfigurationWidget::saveConfiguration()
{
	foreach (Group *group, CurrentContact.groups())
	{
		CurrentContact.removeFromGroup(group);
	}
	foreach (QCheckBox *groupBox, GroupCheckBoxList)
	{
		if (groupBox->isChecked())
			CurrentContact.addToGroup(GroupManager::instance()->byName(groupBox->text()));
	}
}
