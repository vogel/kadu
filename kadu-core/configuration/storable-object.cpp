/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xml_config_file.h"

#include "storable-object.h"

StoragePoint * StorableObject::storage()
{
	if (!Storage)
		Storage = createStoragePoint();

	return Storage;
}

void StorableObject::storeValue(const QString &name, const QVariant value)
{
	Storage->storage()->createTextNode(Storage->point(), name, value.toString());
}
