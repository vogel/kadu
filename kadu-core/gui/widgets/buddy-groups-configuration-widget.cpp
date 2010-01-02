/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-contact-data-manager.h"
#include "contacts/contact.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "misc/misc.h"

#include "buddy-groups-configuration-widget.h"

BuddyGroupsConfigurationWidget::BuddyGroupsConfigurationWidget(Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyGroupsConfigurationWidget::~BuddyGroupsConfigurationWidget()
{
}

void BuddyGroupsConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *label = new QLabel(tr("Add <b>%1</b> to the groups below by checking the box next to the appropriate groups.").arg(MyBuddy.display()), this);
	label->setWordWrap(true);

	layout->addWidget(label);
	layout->addSpacing(64);

	Groups = new QScrollArea(this);
	layout->addWidget(Groups);

	Groups->setFrameShape(QFrame::NoFrame);
	Groups->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	Groups->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QVBoxLayout *groupsLayout = new QVBoxLayout(Groups);

	foreach (Group group, GroupManager::instance()->items())
	{
		QCheckBox *groupCheckBox = new QCheckBox(group.name(), Groups);
		groupCheckBox->setChecked(MyBuddy.isInGroup(group));
		groupsLayout->addWidget(groupCheckBox);
		GroupCheckBoxList.append(groupCheckBox);
	}

	groupsLayout->addStretch(100);
}

void BuddyGroupsConfigurationWidget::save()
{
	foreach (Group group, MyBuddy.groups())
		MyBuddy.removeFromGroup(group);

	foreach (QCheckBox *groupBox, GroupCheckBoxList)
		if (groupBox->isChecked())
			MyBuddy.addToGroup(GroupManager::instance()->byName(groupBox->text()));
}
