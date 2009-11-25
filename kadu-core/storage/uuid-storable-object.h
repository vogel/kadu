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

#include "exports.h"

class KADUAPI UuidStorableObject : public StorableObject
{

protected:
	virtual StoragePoint * createStoragePoint();

public:
	explicit UuidStorableObject(const QString &nodeName, StorableObjectState state = StateUnloaded);
	explicit UuidStorableObject(StoragePoint *storage);
	UuidStorableObject(const QString &nodeName, StorableObject *parent, StorableObjectState state = StateUnloaded);

	virtual QUuid uuid() const = 0;

};

#endif // UUID_STORABLE_OBJECT_H
