/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

void BaseStatusContainer::setStatus(Status status)
{
	doSetStatus(status);
	storeStatus(status);

//	TODO: this cannot be called here, causes accounts to be removed
// 	ConfigurationManager::instance()->flush();
}

void BaseStatusContainer::setDescription(const QString &description)
{
	Status currentStatus = status();
	currentStatus.setDescription(description);
	setStatus(currentStatus);
}

void BaseStatusContainer::setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
		const QString &startupDescription, bool StartupLastDescription)
{
	if (!MyStorableObject->isValidStorage())
		return;

	QString description;
	if (StartupLastDescription)
		description = MyStorableObject->loadValue<QString>("LastStatusDescription");
	else
		description = startupDescription;

	QString name;
	if (startupStatus == "LastStatus")
	{
		name = MyStorableObject->loadValue<QString>("LastStatusName");
		if (name.isEmpty())
			name = "Online";
		else if ("Offline" == name && offlineToInvisible)
			name = "Invisible";
	}
	else
		name = startupStatus;

	if ("Offline" == name && offlineToInvisible)
		name = "Invisible";

	Status status;
	status.setType(name);
	status.setDescription(description);

	setStatus(status);
}

void BaseStatusContainer::storeStatus(Status status)
{
	if (!MyStorableObject->isValidStorage())
		return;

	MyStorableObject->storeValue("LastStatusDescription", status.description());
	MyStorableObject->storeValue("LastStatusName", status.type());
}

void BaseStatusContainer::disconnectStatus(bool disconnectWithCurrentDescription,
		const QString &disconnectDescription)
{
	if (status().type() == "Offline")
		return;

	Status disconnectStatus;
	disconnectStatus.setType("Offline");
	QString description;
	if (disconnectWithCurrentDescription)
		description = status().description();
	else
		description = disconnectDescription;

	disconnectStatus.setDescription(description);
	doSetStatus(disconnectStatus); // this does not stores status to configuration
}
