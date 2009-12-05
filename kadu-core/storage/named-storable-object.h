/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NAMED_STORABLE_OBJECT_H
#define NAMED_STORABLE_OBJECT_H

#include "storable-object.h"

#include "exports.h"

class KADUAPI NamedStorableObject : public StorableObject
{

protected:
	virtual StoragePoint * createStoragePoint();

public:
	NamedStorableObject();

	virtual QString name() const = 0;

};

#endif // NAMED_STORABLE_OBJECT_H
