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
	Name = name;
	Icon = config_file.readEntry("GroupIcon", name);
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
}
