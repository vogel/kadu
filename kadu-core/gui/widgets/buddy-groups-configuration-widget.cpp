/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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
#include <QtGui/QVBoxLayout>

#include "buddies/group.h"
#include "contacts/contact.h"
#include "gui/widgets/group-list.h"
#include "misc/misc.h"

#include "buddy-groups-configuration-widget.h"

BuddyGroupsConfigurationWidget::BuddyGroupsConfigurationWidget(const Buddy &buddy, QWidget *parent) :
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

	BuddyGroupList = new GroupList(this);
	BuddyGroupList->setCheckedGroups(MyBuddy.groups());

	layout->addWidget(label);
	layout->addWidget(BuddyGroupList);
}

void BuddyGroupsConfigurationWidget::save()
{
	MyBuddy.setGroups(BuddyGroupList->checkedGroups());
}

#include "moc_buddy-groups-configuration-widget.cpp"
