/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "notification/notification-service.h"

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
	Group group{sender()};
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
