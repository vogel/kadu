/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storable-object.h"

StorableObject::StorableObject(StorableObjectState state) :
		Parent(0), Storage(0), State(state)
{
}

StorableObject::StorableObject(StoragePoint *storage, StorableObjectState state) :
		Parent(0), Storage(storage), State(state)
{
}

StorableObject::StorableObject(const QString &nodeName, StorableObjectState state) :
		Parent(0), NodeName(nodeName), Storage(0), State(state)
{
}

StorableObject::StorableObject(const QString &nodeName, StorableObject *parent, StorableObjectState state) :
		Parent(parent), NodeName(nodeName), Storage(0), State(state)
{
}

StoragePoint * StorableObject::createStoragePoint()
{
	if (!Parent)
		return new StoragePoint(xml_config_file, xml_config_file->getNode(NodeName));

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

void StorableObject::load()
{
	State = StateLoaded;
}

bool StorableObject::needsLoad()
{
	return StateUnloaded == State;
}

void StorableObject::ensureLoaded()
{
	if (needsLoad())
		load();
}

void StorableObject::removeFromStorage()
{
	if (!Storage)
		return;

	Storage->point().parentNode().removeChild(Storage->point());
	delete Storage;
	Storage = 0;
}

void StorableObject::storeValue(const QString &name, const QVariant value, bool attribute)
{
	if (attribute)
		Storage->point().setAttribute(name, value.toString());
	else
		Storage->storage()->createTextNode(Storage->point(), name, value.toString());
}

void StorableObject::removeValue(const QString& name, bool attribute)
{
	if (attribute)
		Storage->point().removeAttribute(name);
	else
		Storage->storage()->removeNode(Storage->point(), name);
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

