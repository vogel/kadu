/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"

#include "contact-module-data.h"

ContactModuleData::ContactModuleData(StoragePoint *storage)
	: Storage(storage)
{
}

ContactModuleData::~ContactModuleData()
{
	if (Storage)
	{
		delete Storage;
		Storage = 0;
	}
}
