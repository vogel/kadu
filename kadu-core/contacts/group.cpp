/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"
#include "configuration/storage-point.h"
#include "configuration/xml-configuration-file.h"

#include "group-manager.h"

#include "group.h"

Group::Group(StoragePoint *storagePoint) :
		UuidStorableObject(storagePoint)
{
}

Group::Group(QUuid uuid) :
		UuidStorableObject("Group", GroupManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Group::~Group()
{
}

Group * Group::loadFromStorage(StoragePoint *groupStoragePoint)
{
	Group *group = new Group(groupStoragePoint);
	group->load();

	return group;
}

void Group::importConfiguration(const QString &name)
{
	Name = name;
	Icon = config_file.readEntry("GroupIcon", name);
	NotifyAboutStatusChanges = true;
	ShowInAllGroup = true;
	OfflineToGroup = false;
	ShowIcon = !Icon.isEmpty(); 
	ShowName = true;
}

void Group::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	Uuid = loadAttribute<QString>("uuid");
	Name = loadValue<QString>("Name");
	Icon = loadValue<QString>("Icon");
	NotifyAboutStatusChanges = loadValue<bool>("NotifyAboutStatusChanges", true);
	ShowInAllGroup = loadValue<bool>("ShowInAllGroup", true);
	OfflineToGroup = loadValue<bool>("OfflineTo", true);
	ShowIcon = loadValue<bool>("ShowIcon", true);
	ShowName = loadValue<bool>("ShowName", true);
}

void Group::store()
{
	if (!isValidStorage())
		return;

	storeValue("Name", Name);
	storeValue("Icon", Icon);
	storeValue("NotifyAboutStatusChanges", NotifyAboutStatusChanges);
	storeValue("ShowInAllGroup", ShowInAllGroup);
	storeValue("OfflineTo", OfflineToGroup);
	storeValue("ShowIcon", ShowIcon);
	storeValue("ShowName", ShowName);
}

void Group::setName(const QString &name)
{
	Name = name;
	emit nameChanged(this);
}

void Group::setAppearance(bool showName, bool showIcon, const QString &icon)
{
	Icon = icon;
	ShowIcon = showIcon;
	ShowName = showName;

	emit appearanceChanged(this);
}
//TODO 0.6.6:
void Group::setNotifyAboutStatuses(bool notify)
{
	NotifyAboutStatusChanges = notify;
}
void Group::setOfflineTo(bool offline)
{
	OfflineToGroup = offline;
}
void Group::setShowInAllGroup(bool show)
{
	ShowInAllGroup = show;
	emit showInAllChanged();

}
