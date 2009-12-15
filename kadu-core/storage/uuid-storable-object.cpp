/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "uuid-storable-object.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs object with StateNew state and null storage point.
 *
 * Constructs object with @link<StorableObject::StateNew state @endlink and null
 * (invalid) @link<StorableObject::storage storage point @endlink.
 */
UuidStorableObject::UuidStorableObject()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates default storage point for object.
 *
 * Constructs storage point: XML node that is child of storage point of object
 * returned by @link<StorableObject::storageParent storageParent @endlink method.
 * Node name is given by @link<StorableObject::storageNodeName @endlink method.
 * Node contains one attribute: uuid, that contains value returned by
 * @link<uuid> uuid @endlink method.
 *
 * If @link<StorableObject::storageNodeName @endlink returns invalid node name
 * (empty string) or @link<StorableObject::storageParent storageParent @endlink
 * returns object that has invalid storage point, this method returns invalid
 * storage point.
 *
 * If parent is NULL this method will return storage point that is child of
 * root node of XML configuration file.
 */
StoragePoint * UuidStorableObject::createStoragePoint()
{
	if (storageNodeName().isEmpty())
		return 0;

	StorableObject *parent = storageParent();
	if (!parent)
		return 0;

	StoragePoint *parentStoragePoint = storageParent()->storage();
	if (!parentStoragePoint)
		return 0;

	QUuid id = uuid();
	if (id.isNull())
		return 0;

	QDomElement node = parentStoragePoint->storage()->getUuidNode(parentStoragePoint->point(), storageNodeName(), id);
	return new StoragePoint(parentStoragePoint->storage(), node);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Determines if object is worth to be stored.
 * @return true if object should be stored, defaults to true
 * @todo this method is used only in managers, in 0.8 it should be used in every place
 *
 * Returns true if object has valid uuid. Otherwise object will not be stored.
 */
bool UuidStorableObject::shouldStore()
{
	return StorableObject::shouldStore() && !uuid().isNull();
}
