/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UUID_STORABLE_OBJECT_H
#define UUID_STORABLE_OBJECT_H

#include <QtCore/QUuid>

#include "storable-object.h"

class UuidStorableObject : public StorableObject
{

protected:
	virtual StoragePoint * createStoragePoint();

public:
	UuidStorableObject(const QString &nodeName, StorableObject *parent);

	virtual QUuid uuid() const = 0;

};

#endif // UUID_STORABLE_OBJECT_H
