/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/services/file-transfer-service.h"
#include "protocols/protocol.h"

#include "file-transfer-shared.h"

FileTransferShared * FileTransferShared::loadFromStorage(StoragePoint *fileTransferStoragePoint)
{
	FileTransferShared *result = new FileTransferShared();
	result->setStorage(fileTransferStoragePoint);
	return result;
}

FileTransferShared::FileTransferShared(QUuid uuid) :
		QObject(FileTransferManager::instance()), Shared(uuid),
		FileSize(0), TransferredSize(0),
		TransferType(TypeReceive), TransferStatus(StatusNotConnected),
		TransferError(ErrorOk), Handler(0)
{
}

FileTransferShared::~FileTransferShared()
{
}

StorableObject * FileTransferShared::storageParent()
{
	return FileTransferManager::instance();
}

QString FileTransferShared::storageNodeName()
{
	return QLatin1String("FileTransfer");
}

void FileTransferShared::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Peer = ContactManager::instance()->byUuid(loadValue<QString>("Peer"), true);
	LocalFileName = loadValue<QString>("LocalFileName");
	RemoteFileName = loadValue<QString>("RemoteFileName");
	TransferType = ("Send" == loadValue<QString>("TransferType")) ? TypeSend : TypeReceive;
	FileSize = loadValue<qulonglong>("FileSize");
	TransferredSize = loadValue<qulonglong>("TransferredSize");

	if (FileSize == TransferredSize)
		setTransferStatus(StatusFinished);
}

void FileTransferShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Peer", Peer.uuid().toString());
	storeValue("LocalFileName", LocalFileName);
	storeValue("RemoteFileName", RemoteFileName);
	storeValue("TransferType", TypeSend == TransferType ? "Send" : "Receive");
	storeValue("FileSize", (qulonglong)FileSize);
	storeValue("TransferredSize", (qulonglong)TransferredSize);
}

void FileTransferShared::setTransferStatus(FileTransferStatus transferStatus)
{
	if (TransferStatus == transferStatus)
		return;

	TransferStatus = transferStatus;
	emit statusChanged();
	dataUpdated();
}

void FileTransferShared::setTransferError(FileTransferError transferError)
{
	if (TransferStatus == StatusNotConnected && TransferError == transferError)
		return;

	TransferStatus = StatusNotConnected;
	TransferError = transferError;
	emit statusChanged();
	dataUpdated();
}

void FileTransferShared::setHandler(FileTransferHandler *handler)
{
	if (Handler == handler)
		return;

	if (Handler)
		disconnect(Handler, SIGNAL(destroyed(QObject *)), this, SLOT(handlerDestroyed()));

	Handler = handler;
	connect(Handler, SIGNAL(destroyed(QObject *)), this, SLOT(handlerDestroyed()));
	dataUpdated();
}

void FileTransferShared::createHandler()
{
	if (Handler)
		return;

	Protocol *protocol = Peer.contactAccount().protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	Handler = service->createFileTransferHandler(this);
}

void FileTransferShared::emitUpdated()
{
	emit updated();
}

void FileTransferShared::handlerDestroyed()
{
	setHandler(0);
}
