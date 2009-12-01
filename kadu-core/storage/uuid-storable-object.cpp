/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "uuid-storable-object.h"

UuidStorableObject::UuidStorableObject()
{
}

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
