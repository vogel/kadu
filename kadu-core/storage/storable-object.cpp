/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storable-object.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs object with 'new' state and null storage point.
 *
 * Constructs object with @link<StorableObject::StateNew state @endlink and null
 * (invalid) @link<StorableObject::storage storage point @endlink.
 */
StorableObject::StorableObject() :
		Storage(0), State(StateNew)
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates default storage point for object.
 *
 * Constructs storage point: XML node that is child of storage point of object
 * returned by @link<StorableObject::storageParent storageParent @endlink method.
 * Node name is given by @link<StorableObject::storageNodeName @endlink method.
 *
 * If @link<StorableObject::storageNodeName @endlink returns invalid node name
 * (empty string) or @link<StorableObject::storageParent storageParent @endlink
 * returns object that has invalid storage point, this method returns invalid
 * storage point.
 *
 * If parent is NULL this method will return storage point that is child of
 * root node of XML configuration file.
 */
StoragePoint * StorableObject::createStoragePoint()
{
	if (storageNodeName().isEmpty())
		return 0;

	StorableObject *parent = storageParent();
	if (!parent)
		return new StoragePoint(xml_config_file, xml_config_file->getNode(storageNodeName()));

	StoragePoint *parentStoragePoint = storageParent()->storage();
	if (!parentStoragePoint)
		return 0;

	QDomElement node = parentStoragePoint->storage()->getNode(parentStoragePoint->point(), storageNodeName());
	return new StoragePoint(parentStoragePoint->storage(), node);
}

void StorableObject::setStorage(StoragePoint *storage)
{
	State = StateNotLoaded;
	Storage = storage;
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

void StorableObject::ensureLoaded()
{
	if (StateNotLoaded == State)
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
