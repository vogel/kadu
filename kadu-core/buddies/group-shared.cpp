/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"

#include "group-shared.h"

GroupShared * GroupShared::loadFromStorage(StoragePoint *storagePoint)
{
	GroupShared *result = new GroupShared();
	result->setStorage(storagePoint);
	result->load();

	return result;
}

GroupShared::GroupShared(QUuid uuid) :
		Shared(uuid),
		NotifyAboutStatusChanges(false), ShowInAllGroup(false),
		OfflineToGroup(false), ShowIcon(false), ShowName(false),
		TabPosition(-1)
{
}

GroupShared::~GroupShared()
{
}

StorableObject * GroupShared::storageParent()
{
	return GroupManager::instance();
}

QString GroupShared::storageNodeName()
{
	return QLatin1String("Group");
}

void GroupShared::importConfiguration(const QString &name)
{
	Name = name;
	Icon = config_file.readEntry("GroupIcon", name);
	NotifyAboutStatusChanges = true;
	ShowInAllGroup= true;
	OfflineToGroup= false;
	ShowIcon = !Icon.isEmpty();
	ShowName = true;
	TabPosition = -1;
}

void GroupShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Name = loadValue<QString>("Name");
	Icon = loadValue<QString>("Icon");
	NotifyAboutStatusChanges = loadValue<bool>("NotifyAboutStatusChanges", true);
	ShowInAllGroup= loadValue<bool>("ShowInAllGroup", true);
	OfflineToGroup= loadValue<bool>("OfflineTo", true);
	ShowIcon = loadValue<bool>("ShowIcon", true);
	ShowName = loadValue<bool>("ShowName", true);
	TabPosition = loadValue<int>("TabPosition", -1);
}

void GroupShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("Name", Name);
	storeValue("Icon", Icon);
	storeValue("NotifyAboutStatusChanges", NotifyAboutStatusChanges);
	storeValue("ShowInAllGroup", ShowInAllGroup);
	storeValue("OfflineTo", OfflineToGroup);
	storeValue("ShowIcon", ShowIcon);
	storeValue("ShowName", ShowName);
	storeValue("TabPosition", TabPosition);
}

void GroupShared::emitUpdated()
{
	emit updated();
}
