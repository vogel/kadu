/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-groups-configuration-widget.h"

#include "buddies/group.h"
#include "contacts/contact.h"
#include "gui/widgets/group-list.h"
#include "misc/misc.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

BuddyGroupsConfigurationWidget::BuddyGroupsConfigurationWidget(const Buddy &buddy, QWidget *parent) :
		QWidget{parent},
		m_buddy{buddy}
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyGroupsConfigurationWidget::~BuddyGroupsConfigurationWidget()
{
}

void BuddyGroupsConfigurationWidget::createGui()
{
	auto layout = new QVBoxLayout{this};

	QLabel *label = new QLabel{tr("Add <b>%1</b> to the groups below by checking the box next to the appropriate groups.").arg(m_buddy.display()), this};
	label->setWordWrap(true);

	m_groupList = new GroupList{this};
	m_groupList->setCheckedGroups(m_buddy.groups());

	layout->addWidget(label);
	layout->addWidget(m_groupList);
}

void BuddyGroupsConfigurationWidget::save()
{
	m_buddy.setGroups(m_groupList->checkedGroups());
}

#include "moc_buddy-groups-configuration-widget.cpp"
