/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_SHARED_H
#define FILE_TRANSFER_SHARED_H

#include "accounts/account.h"
#include "file-transfer/file-transfer.h"
#include "storage/shared.h"

class KADUAPI FileTransferShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferShared)

	Account FileTransferAccount;
	Contact FileTransferContact;
	QString LocalFileName;
	QString RemoteFileName;

	unsigned long FileSize;
	unsigned long TransferredSize;

	FileTransfer::FileTransferType TransferType;
	FileTransfer::FileTransferStatus TransferStatus;
	FileTransfer::FileTransferError TransferError;

protected:
	virtual void load();
	virtual void emitUpdated();

public:
	static FileTransferShared * loadFromStorage(StoragePoint *fileTransferStoragePoint);

	explicit FileTransferShared(QUuid uuid = QUuid());
	virtual ~FileTransferShared();

	virtual void store();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setTransferStatus(FileTransfer::FileTransferStatus transferStatus);

	KaduShared_Property(Account, fileTransferAccount, FileTransferAccount)
	KaduShared_Property(Contact, fileTransferContact, FileTransferContact)
	KaduShared_Property(QString, localFileName, LocalFileName)
	KaduShared_Property(QString, remoteFileName, RemoteFileName)
	KaduShared_Property(unsigned long, fileSize, FileSize)
	KaduShared_Property(unsigned long, transferredSize, TransferredSize)
	KaduShared_Property(FileTransfer::FileTransferType, transferType, TransferType)
	KaduShared_PropertyRead(FileTransfer::FileTransferStatus, transferStatus, TransferStatus)
	KaduShared_Property(FileTransfer::FileTransferError, transferError, TransferError)

signals:
	void statusChanged();

	void updated();

};

#endif // FILE_TRANSFER_SHARED_H

