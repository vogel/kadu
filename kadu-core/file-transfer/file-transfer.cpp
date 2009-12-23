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
#include "configuration/xml-configuration-file.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"

#include "file-transfer.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"

KaduSharedBaseClassImpl(FileTransfer)

FileTransfer FileTransfer::null;

FileTransfer FileTransfer::create()
{
	return new FileTransferShared();
}

FileTransfer FileTransfer::loadFromStorage(StoragePoint *fileTransferStoragePoint)
{
	return FileTransferShared::loadFromStorage(fileTransferStoragePoint);
}

FileTransfer::FileTransfer()
{
}

FileTransfer::FileTransfer(FileTransferShared *data) :
		SharedBase<FileTransferShared>(data)
{
	data->ref.ref();
}

FileTransfer::FileTransfer(QObject *data)
{
	FileTransferShared *shared = dynamic_cast<FileTransferShared *>(data);
	if (shared)
		setData(shared);
}

FileTransfer::FileTransfer(const FileTransfer &copy) :
		SharedBase<FileTransferShared>(copy)
{
}

FileTransfer::~FileTransfer()
{
}

unsigned int FileTransfer::percent()
{
	if (fileSize() != 0)
		return (100 * transferredSize()) / fileSize();
	else
		return 0;
}

bool FileTransfer::accept(const QFile &file)
{
	setLocalFileName(file.fileName());
	// XXX
	return true;
}

KaduSharedBase_PropertyDef(FileTransfer, Account, fileTransferAccount, FileTransferAccount, Account::null)
KaduSharedBase_PropertyDef(FileTransfer, Contact, fileTransferContact, FileTransferContact, Contact::null)
KaduSharedBase_PropertyDef(FileTransfer, QString, localFileName, LocalFileName, QString::null)
KaduSharedBase_PropertyDef(FileTransfer, QString, remoteFileName, RemoteFileName, QString::null)
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, fileSize, FileSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, transferredSize, TransferredSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferType, transferType, TransferType, TypeSend)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferStatus, transferStatus, TransferStatus, StatusNotConnected)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferError, transferError, TransferError, ErrorOk)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferHandler *, handler, Handler, 0)
