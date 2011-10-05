/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "storage/storable-object.h"

#include "base-status-container.h"

BaseStatusContainer::BaseStatusContainer(StorableObject *storableObject) :
		MyStorableObject(storableObject)
{
}

BaseStatusContainer::~BaseStatusContainer()
{
}

Status BaseStatusContainer::loadStatus()
{
	if (!MyStorableObject->isValidStorage())
		return Status();

	QString name = MyStorableObject->loadValue<QString>("LastStatusName");
	QString description = MyStorableObject->loadValue<QString>("LastStatusDescription");

	Status status;
	status.setType(name);
	status.setDescription(description);

	return status;
}

void BaseStatusContainer::storeStatus(Status status)
{
	if (!MyStorableObject->isValidStorage())
		return;

	MyStorableObject->storeValue("LastStatusDescription", status.description());
	MyStorableObject->storeValue("LastStatusName", status.type());

	ConfigurationManager::instance()->flush();
}
