/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storable-object.h"

StorableObject::StorableObject() :
		Parent(0), Storage(0)
{
}

StorableObject::StorableObject(const QString &nodeName, StorableObject *parent) :
		Parent(parent), NodeName(nodeName), Storage(0)
{
}

StoragePoint * StorableObject::createStoragePoint()
{
	if (!Parent)
		return 0;

	StoragePoint *parentStoragePoint = Parent->storage();
	if (!parentStoragePoint)
		return 0;

	QDomElement node = parentStoragePoint->storage()->getNode(parentStoragePoint->point(), NodeName);
	return new StoragePoint(parentStoragePoint->storage(), node);
}

StoragePoint * StorableObject::storage()
{
	if (!Storage)
		Storage = createStoragePoint();

	return Storage;
}

void StorableObject::removeFromStorage()
{
	if (!Storage)
		return;

	Storage->point().parentNode().removeChild(Storage->point());
	delete Storage;
	Storage = 0;
}

void StorableObject::storeValue(const QString &name, const QVariant value)
{
	Storage->storage()->createTextNode(Storage->point(), name, value.toString());
}

void StorableObject::storeModuleData()
{
	foreach (ModuleData *moduleData, ModulesData.values())
		moduleData->storeConfiguration();
}

StoragePoint * StorableObject::storagePointForModuleData(const QString &module, bool create)
{
	StoragePoint *parent = storage();
	if (!parent || !parent->storage())
		return 0;

	QDomElement moduleDataNode = parent->storage()->getNamedNode(parent->point(), "ModuleData",
			module, create ? XmlConfigFile::ModeGet : XmlConfigFile::ModeFind);
	return moduleDataNode.isNull()
		? 0
		: new StoragePoint(parent->storage(), moduleDataNode);
}

