/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "module-data.h"

ModuleData::ModuleData(StorableObject *storageParent)
		: CurrentStorageParent(storageParent)
{
}

StorableObject * ModuleData::storageParent()
{
	return CurrentStorageParent;
}

QString ModuleData::storageNodeName()
{
	return QLatin1String("ModuleData");
}
