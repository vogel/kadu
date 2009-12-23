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
#include "file-transfer/file-transfer-manager.h"

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
		TransferError(ErrorOk)
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

	FileTransferAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"), true);
	FileTransferContact = ContactManager::instance()->byUuid(loadValue<QString>("Peer"), true);
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

	storeValue("Account", FileTransferAccount.uuid().toString());
	storeValue("Peer", FileTransferContact.uuid().toString());
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

void FileTransferShared::emitUpdated()
{
	emit updated();
}
