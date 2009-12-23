/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-handler.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"

#include "modules.h"

#include "file-transfer-manager.h"

FileTransferManager * FileTransferManager::Instance = 0;

FileTransferManager * FileTransferManager::instance()
{
	if (0 == Instance)
		Instance = new FileTransferManager();

	return Instance;
}

FileTransferManager::FileTransferManager()
{
}

FileTransferManager::~FileTransferManager()
{
}

void FileTransferManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	connect(service, SIGNAL(incomingFileTransfer(FileTransfer *)),
			this, SLOT(incomingFileTransfer(FileTransfer *)));
}

void FileTransferManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	disconnect(service, SIGNAL(incomingFileTransfer(FileTransfer *)),
			this, SLOT(incomingFileTransfer(FileTransfer *)));
}

void FileTransferManager::cleanUp()
{
	// TODO: current
// 	foreach (FileTransfer fileTransfer, items())
// 		if (FileTransfer::StatusFinished == fileTransfer->transferStatus())
// 			removeFileTransfer(fileTransfer);
}

void FileTransferManager::incomingFileTransfer(FileTransfer fileTransfer)
{
	if (!ModulesManager::instance()->loadedModules().contains("file_transfer"))
		if (fileTransfer.handler())
			fileTransfer.handler()->reject();

	emit incomingFileTransferNeedAccept(fileTransfer);
}

void FileTransferManager::itemAboutToBeAdded(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeAdded(fileTransfer);
}

void FileTransferManager::itemAdded(FileTransfer fileTransfer)
{
	emit fileTransferAdded(fileTransfer);
}

void FileTransferManager::itemAboutToBeRemoved(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeRemoved(fileTransfer);
}

void FileTransferManager::itemRemoved(FileTransfer fileTransfer)
{
	emit fileTransferRemoved(fileTransfer);
}
