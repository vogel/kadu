/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "storage/storable-object.h"

#include "storable-status-container.h"

StorableStatusContainer::StorableStatusContainer(StorableObject *storableObject) :
		MyStorableObject(storableObject)
{
}

StorableStatusContainer::~StorableStatusContainer()
{
}

Status StorableStatusContainer::loadStatus()
{
	if (!MyStorableObject->isValidStorage())
		return Status();

	QString name = MyStorableObject->loadValue<QString>("LastStatusName");
	QString description = MyStorableObject->loadValue<QString>("LastStatusDescription");

	// if no status is available in storage, then this status container is a new one
	// so we need to connect ASAP
	if (name.isEmpty())
		name = "Online";

	Status status;
	status.setType(StatusTypeManager::instance()->fromName(name));
	status.setDescription(description);

	return status;
}

void StorableStatusContainer::storeStatus(Status status)
{
	if (!MyStorableObject->isValidStorage())
		return;

	MyStorableObject->storeValue("LastStatusDescription", status.description());
	MyStorableObject->storeValue("LastStatusName", StatusTypeManager::instance()->statusTypeData(status.type()).name());

	ConfigurationManager::instance()->flush();
}

#include "moc_storable-status-container.cpp"
