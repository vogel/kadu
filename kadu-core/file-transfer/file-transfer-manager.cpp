/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"
#include "file-transfer/file-transfer.h"

#include "xml_config_file.h"

#include "file-transfer-manager.h"

FileTransferManager * FileTransferManager::Instance = 0;

FileTransferManager * FileTransferManager::instance()
{
	if (!Instance)
		Instance = new FileTransferManager();

	return Instance;
}

FileTransferManager::FileTransferManager()
{
}

FileTransferManager::~FileTransferManager()
{
}

StoragePoint * FileTransferManager::createStoragePoint() const
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("FileTransfersNew"));
}

void FileTransferManager::loadConfiguration()
{
}

void FileTransferManager::storeConfiguration()
{
	printf("storing all\n");

	foreach (FileTransfer *fileTransfer, FileTransfers)
		fileTransfer->storeConfiguration();
}

void FileTransferManager::addFileTransfer(FileTransfer *fileTransfer)
{
	emit fileTransferAboutToBeAdded(fileTransfer);
	FileTransfers.append(fileTransfer);
	emit fileTransferAdded(fileTransfer);
}

void FileTransferManager::removeFileTransfer(FileTransfer *fileTransfer)
{
	emit fileTransferAboutToBeRemoved(fileTransfer);
	FileTransfers.removeAll(fileTransfer);
	emit fileTransferRemoved(fileTransfer);
}

void FileTransferManager::cleanUp()
{
	foreach (FileTransfer *fileTransfer, FileTransfers)
		if (FileTransfer::StatusFinished == fileTransfer->transferStatus())
		{
			removeFileTransfer(fileTransfer);
			fileTransfer->deleteLater();
		}
}
