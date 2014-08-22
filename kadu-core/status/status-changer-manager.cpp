/*
 * %kadu copyright begin%
 * Copyright 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "debug.h"

#include "status-changer-manager.h"

StatusChangerManager * StatusChangerManager::Instance = 0;

StatusChangerManager * StatusChangerManager::instance()
{
	if (0 == Instance)
		Instance = new StatusChangerManager();

	return Instance;
}

StatusChangerManager::StatusChangerManager()
{
}

StatusChangerManager::~StatusChangerManager()
{
}

void StatusChangerManager::registerStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	connect(statusChanger, SIGNAL(statusChanged(StatusContainer *)), this, SLOT(statusChanged(StatusContainer *)));

	for (int i = 0; i < StatusChangers.count(); i++)
		if (StatusChangers.at(i)->priority() > statusChanger->priority())
		{
			StatusChangers.insert(i, statusChanger);
			statusChanged();
			kdebugf2();
			return;
		}

	StatusChangers.append(statusChanger);
	statusChanged();

	kdebugf2();
}

void StatusChangerManager::unregisterStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	if (StatusChangers.removeAll(statusChanger))
	{
		disconnect(statusChanger, 0, this, 0);
		statusChanged();
	}

	kdebugf2();
}

void StatusChangerManager::setStatus(StatusContainer *statusContainer, Status status)
{
	if (statusContainer)
	{
		Statuses.insert(statusContainer, status);
		statusChanged(statusContainer, SourceUser);
	}
}

Status StatusChangerManager::manuallySetStatus(StatusContainer *statusContainer)
{
	if (Statuses.contains(statusContainer))
		return Statuses.value(statusContainer);
	return Status();
}

void StatusChangerManager::statusChanged(StatusContainer *container, StatusChangeSource source)
{
	if (!container)
	{
		foreach (StatusContainer *statusContainer, StatusContainerManager::instance()->statusContainers())
			if (statusContainer)
				statusChanged(statusContainer, source);

		return;
	}

	if (Statuses.contains(container))
	{
		Status status = Statuses.value(container);
		for (int i = 0; i < StatusChangers.count(); i++)
			StatusChangers.at(i)->changeStatus(container, status);

		if (SourceUser == source || container->status() != status)
			container->setStatus(status, source);
	}
}

#include "moc_status-changer-manager.cpp"
