/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"

#include "module-data.h"

ModuleData::ModuleData(StoragePoint *storage)
	: Storage(storage)
{
}

ModuleData::~ModuleData()
{
	if (Storage)
	{
		delete Storage;
		Storage = 0;
	}
}
