/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFile>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/storage-point.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "xml_config_file.h"

#include "file-transfer.h"
#include <contacts/contact-manager.h>
#include <contacts/contact-manager.h>

FileTransfer::FileTransfer(Account *account) :
		Uuid(QUuid::createUuid()), CurrentAccount(account), Peer(Contact::null),
		FileSize(0), TransferredSize(0),
		TransferType(TypeReceive), TransferStatus(StatusNotConnected), TransferError(ErrorOk)
{
}

FileTransfer::FileTransfer(Account *account, Contact peer, FileTransferType transferType) :
		Uuid(QUuid::createUuid()), CurrentAccount(account), Peer(peer),
		FileSize(0), TransferredSize(0),
		TransferType(transferType), TransferStatus(StatusNotConnected), TransferError(ErrorOk)
{
}

FileTransfer::~FileTransfer()
{
}

FileTransfer * FileTransfer::loadFromStorage(StoragePoint *fileTransferStoragePoint)
{
	if (!fileTransferStoragePoint || !fileTransferStoragePoint->storage())
		return 0;

	XmlConfigFile *storage = fileTransferStoragePoint->storage();
	QDomElement point = fileTransferStoragePoint->point();

	Account *account = AccountManager::instance()->byUuid(QUuid(storage->getTextNode(point, "Account")));
	if (!account)
		return 0;

	FileTransferService *service = account->protocol()->fileTransferService();
	if (!service)
		return 0;

	FileTransfer *ft = service->loadFileTransferFromStorage(fileTransferStoragePoint);
	if (!ft)
		return 0;

	return ft;
}

StoragePoint * FileTransfer::createStoragePoint()
{
	StoragePoint *parent = FileTransferManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement contactNode = parent->storage()->getUuidNode(parent->point(), "FileTransfer", Uuid.toString());
	return new StoragePoint(parent->storage(), contactNode);
}

void FileTransfer::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	CurrentAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));
	Peer = ContactManager::instance()->byUuid(loadValue<QString>("Peer"));
	LocalFileName = loadValue<QString>("LocalFileName");
	RemoteFileName = loadValue<QString>("RemoteFileName");
	TransferType = ("Send" == loadValue<QString>("TransferType")) ? TypeSend : TypeReceive;
	FileSize = loadValue<qulonglong>("FileSize");
	TransferredSize = loadValue<qulonglong>("TransferredSize");

	if (FileSize == TransferredSize)
		changeFileTransferStatus(StatusFinished);
}

void FileTransfer::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Account", CurrentAccount->uuid().toString());
	storeValue("Peer", Peer.uuid().toString());
	storeValue("LocalFileName", LocalFileName);
	storeValue("RemoteFileName", RemoteFileName);
	storeValue("TransferType", TypeSend == TransferType ? "Send" : "Receive");
	storeValue("FileSize", (qulonglong)FileSize);
	storeValue("TransferredSize", (qulonglong)TransferredSize);
}

void FileTransfer::changeFileTransferStatus(FileTransferStatus transferStatus)
{
	if (TransferStatus == transferStatus)
		return;

	TransferStatus = transferStatus;
	emit statusChanged();
}

void FileTransfer::changeFileTransferError(FileTransferError transferError)
{
	if (TransferStatus == StatusNotConnected && TransferError == transferError)
		return;

	TransferStatus = StatusNotConnected;
	TransferError = transferError;
	emit statusChanged();
}

unsigned int FileTransfer::percent()
{
	if (FileSize != 0)
		return (100 * TransferredSize) / FileSize;
	else
		return 0;
}

bool FileTransfer::accept(const QFile &file)
{
	LocalFileName = file.fileName();
	// XXX
	return true;
}
