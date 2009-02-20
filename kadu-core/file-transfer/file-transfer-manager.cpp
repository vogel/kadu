/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"

#include "xml_config_file.h"

#include "file-transfer-manager.h"
/*
FileTransferManager * FileTransferManager::Instance = 0;

FileTransferManager * FileTransferManager::instance()
{
	if (!Instance)
		Instance = new FileTransferManager();

	return Instance;
}

StoragePoint * FileTransferManager::createStoragePoint() const
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("FileTransfers"));
}

void FileTransferManager::loadConfiguration()
{
}

void FileTransferManager::storeConfiguration()
{
}*/
