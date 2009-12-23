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
	triggerAllAccountsRegistered();
}

FileTransferManager::~FileTransferManager()
{
	triggerAllAccountsUnregistered();
}

void FileTransferManager::accountRegistered(Account account)
{
	printf("Account registered\n");

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	printf("connected\n");
	connect(service, SIGNAL(incomingFileTransfer(FileTransfer)),
			this, SLOT(incomingFileTransfer(FileTransfer)));
}

void FileTransferManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	disconnect(service, SIGNAL(incomingFileTransfer(FileTransfer)),
			this, SLOT(incomingFileTransfer(FileTransfer)));
}

void FileTransferManager::cleanUp()
{
	QList<FileTransfer> toRemove;

	foreach (FileTransfer fileTransfer, items())
		if (StatusFinished == fileTransfer.transferStatus())
			toRemove.append(fileTransfer);

	foreach (FileTransfer fileTransfer, toRemove)
		removeItem(fileTransfer);
}

void FileTransferManager::incomingFileTransfer(FileTransfer fileTransfer)
{
	printf("incoming file transfer\n");

	if (!ModulesManager::instance()->loadedModules().contains("file_transfer"))
	{
		printf("no module loaded\n");
		if (fileTransfer.handler())
			fileTransfer.handler()->reject();
	}

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
