/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "services/notification-service.h"

#include "group-event-listener.h"

GroupEventListener::GroupEventListener(NotificationService *service)
		: EventListener(service)
{
	foreach (const Group &group, GroupManager::instance()->items())
		groupAdded(group);
}

GroupEventListener::~GroupEventListener()
{
}

void GroupEventListener::groupAdded(const Group &group)
{
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));
}

void GroupEventListener::groupUpdated()
{
	Group group = sender();
	if (group.isNull())
		return;

	bool notify = group.notifyAboutStatusChanges();

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		if (notify)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}
}


#include "moc_group-event-listener.cpp"
