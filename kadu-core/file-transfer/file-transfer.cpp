/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "configuration/storage-point.h"
#include "file-transfer/file-transfer-manager.h"
#include "xml_config_file.h"

#include "file-transfer.h"

FileTransfer::FileTransfer(Account *account, Contact peer, FileTransferType transferType) :
		Uuid(QUuid::createUuid()), CurrentAccount(account), Peer(peer),
		TransferType(transferType), TransferStatus(StatusNotConnected), TransferError(ErrorOk)
{
}

FileTransfer::~FileTransfer()
{
	storeConfiguration(); // TODO: 0.6.6 do not du if removed ;D
}

StoragePoint * FileTransfer::createStoragePoint() const
{
	StoragePoint *parent = FileTransferManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement contactNode = parent->storage()->getUuidNode(parent->point(), "FileTransfer", Uuid.toString());
	return new StoragePoint(parent->storage(), contactNode);
}

void FileTransfer::storeConfiguration()
{
	if (!isValidStorage())
		return;

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
