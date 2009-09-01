/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"

#include "base-status-container.h"

BaseStatusContainer::BaseStatusContainer(StoragePoint *storagePoint) :
		UuidStorableObject(storagePoint)
{
}

BaseStatusContainer::BaseStatusContainer(const QString &nodeName, StorableObject *parent) :
		UuidStorableObject(nodeName, parent)
{
}

void BaseStatusContainer::setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription)
{
	if (!isValidStorage())
		return;

	QString description;
    	if (StartupLastDescription)
		description = loadValue<QString>("LastStatusDescription");
	else
		description = startupDescription;

	QString name;
	if (startupStatus == "LastStatus")
	{
		name = loadValue<QString>("LastStatusName");
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

	setPrivateStatus(config_file.readBoolEntry("General", "PrivateStatus"));

	setStatus(status);
}
void BaseStatusContainer::disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription,
						  const QString &disconnectDescription)
{
	if (!isValidStorage())
		return;

	storeValue("LastStatusDescription", status().description());

	storeValue("LastStatusName", statusName());

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
	setStatus(disconnectStatus);
}
