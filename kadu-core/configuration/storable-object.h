/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORABLE_OBJECT_H
#define STORABLE_OBJECT_H

class StoragePoint;

class StorableObject
{
	StoragePoint *Storage;

protected:
	virtual StoragePoint * createStoragePoint() const = 0;

public:
	StorableObject() : Storage(0) {}

	StoragePoint * storage();
	void setStorage(StoragePoint *storage) { Storage = storage; }

};

#endif // STORABLE_OBJECT_H
