/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"

#include "config_file.h"
#include "xml_config_file.h"

#include "group-manager.h"

#include "group.h"

Group::Group(QUuid uuid)
	: Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Group::~Group()
{
}

Group * Group::loadFromStorage(StoragePoint *groupStoragePoint)
{
	Group *group = new Group();
	group->setStorage(groupStoragePoint);
	group->loadConfiguration();

	return group;
}

StoragePoint * Group::createStoragePoint() const
{
	StoragePoint *parent = GroupManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement contactNode = parent->storage()->getUuidNode(parent->point(), "Group", Uuid.toString());
	return new StoragePoint(parent->storage(), contactNode);
}

void Group::importConfiguration(const QString &name)
{
	if (!storage())
		createStoragePoint();
	Name = name;
	Icon = config_file.readEntry("GroupIcon", name);
	NotifyAboutStatusChanges = true;
	ShowInAllGroup = true;
	OfflineToGroup = false;
	ShowIcon = !Icon.isEmpty(); 
	ShowName = true;
}

void Group::loadConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	Uuid = QUuid(parent.attribute("uuid"));
	Name = configurationStorage->getTextNode(parent, "Name");
	Icon = configurationStorage->getTextNode(parent, "Icon");
	QVariant v(configurationStorage->getTextNode(parent, "NotifyAboutStatusChanges", "true"));
	NotifyAboutStatusChanges = v.toBool();
	v.setValue(configurationStorage->getTextNode(parent, "ShowInAllGroup", "true"));
	ShowInAllGroup = v.toBool();
	v.setValue(configurationStorage->getTextNode(parent, "OfflineTo"));
	OfflineToGroup = v.toBool();
	v.setValue(configurationStorage->getTextNode(parent, "ShowIcon"));
	ShowIcon = v.toBool();
	v.setValue(configurationStorage->getTextNode(parent, "ShowName", "true")); 
	ShowName = v.toBool();
}

void Group::storeConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	configurationStorage->createTextNode(parent, "Name", Name);
	configurationStorage->createTextNode(parent, "Icon", Icon);

	QVariant v(NotifyAboutStatusChanges);
	configurationStorage->createTextNode(parent, "NotifyAboutStatusChanges", v.toString());
	v.setValue(ShowInAllGroup);
	configurationStorage->createTextNode(parent, "ShowInAllGroup", v.toString());
	v.setValue(OfflineToGroup);
	configurationStorage->createTextNode(parent, "OfflineTo", v.toString());
	v.setValue(ShowIcon);
	configurationStorage->createTextNode(parent, "ShowIcon", v.toString());
	v.setValue(ShowName);
	configurationStorage->createTextNode(parent, "ShowName", v.toString()); 
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
}
