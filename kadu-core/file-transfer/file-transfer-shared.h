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
#include "file-transfer/file-transfer-enums.h"
#include "storage/shared.h"

class FileTransferHandler;

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

	FileTransferType TransferType;
	FileTransferStatus TransferStatus;
	FileTransferError TransferError;

	FileTransferHandler *Handler;

private slots:
	void handlerDestroyed();

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

	void setTransferStatus(FileTransferStatus transferStatus);
	void setTransferError(FileTransferError transferError);
	void setHandler(FileTransferHandler *handler);
	void createHandler();

	KaduShared_Property(Account, fileTransferAccount, FileTransferAccount)
	KaduShared_Property(Contact, fileTransferContact, FileTransferContact)
	KaduShared_Property(QString, localFileName, LocalFileName)
	KaduShared_Property(QString, remoteFileName, RemoteFileName)
	KaduShared_Property(unsigned long, fileSize, FileSize)
	KaduShared_Property(unsigned long, transferredSize, TransferredSize)
	KaduShared_Property(FileTransferType, transferType, TransferType)
	KaduShared_PropertyRead(FileTransferStatus, transferStatus, TransferStatus)
	KaduShared_PropertyRead(FileTransferError, transferError, TransferError)
	KaduShared_PropertyRead(FileTransferHandler *, handler, Handler)

signals:
	void statusChanged();

	void updated();

};

#endif // FILE_TRANSFER_SHARED_H
