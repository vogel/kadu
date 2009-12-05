/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODULE_DATA_H
#define MODULE_DATA_H

#include <QtCore/QUuid>

#include "named-storable-object.h"

#include "exports.h"

class KADUAPI ModuleData : public NamedStorableObject
{
	StorableObject *CurrentStorageParent;

public:
	ModuleData(StorableObject *storageParent);

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

};

#endif // MODULE_DATA_H
