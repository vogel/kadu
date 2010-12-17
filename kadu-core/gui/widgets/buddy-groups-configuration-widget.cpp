/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

	foreach (const Group &group, GroupManager::instance()->items())
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
	foreach (const Group &group, MyBuddy.groups())
		MyBuddy.removeFromGroup(group);

	foreach (QCheckBox *groupBox, GroupCheckBoxList)
		if (groupBox->isChecked())
			MyBuddy.addToGroup(GroupManager::instance()->byName(groupBox->text()));
}
