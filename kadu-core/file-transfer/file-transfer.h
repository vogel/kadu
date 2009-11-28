/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <QtCore/QObject>

#include "contacts/contact.h"
#include "storage/storable-object.h"

#include "exports.h"

class QFile;

class Account;
class Buddy;

class KADUAPI FileTransfer : public QObject, public StorableObject
{
	Q_OBJECT

public:
	enum FileTransferType {
		TypeSend,
		TypeReceive
	};

	enum FileTransferStatus {
		StatusNotConnected,
		StatusWaitingForConnection,
		StatusWaitingForAccept,
		StatusTransfer,
		StatusFinished,
		StatusRejected
	};

	enum FileTransferError {
		ErrorOk,
		ErrorNetworkError,
		ErrorUnableToOpenFile
	};

	enum StartType {
		StartNew,
		StartRestore
	};

private:
	QUuid Uuid;

	Account CurrentAccount;
	Contact Peer;
	QString LocalFileName;
	QString RemoteFileName;

	unsigned long FileSize;
	unsigned long TransferredSize;

	FileTransferType TransferType;
	FileTransferStatus TransferStatus;
	FileTransferError TransferError;

protected:
	virtual StoragePoint * createStoragePoint();

	void changeFileTransferStatus(FileTransferStatus transferStatus);
	void changeFileTransferError(FileTransferError transferError);

	void setFileSize(unsigned long fileSize) { FileSize = fileSize; }
	void setTransferredSize(unsigned long transferredSize) { TransferredSize = transferredSize; }
	void setRemoteFile(const QString &remoteFileName) { RemoteFileName = remoteFileName; }

	virtual void updateFileInfo() = 0;

public:
	static FileTransfer * loadFromStorage(StoragePoint *fileTransferStoragePoint);

	FileTransfer(Account account);
	FileTransfer(Account account, Contact peer, FileTransferType transferType);
	virtual ~FileTransfer();

	virtual void load();
	virtual void store();

	Account account() { return CurrentAccount; }
	Contact contact() { return Peer; }

	FileTransferType transferType() { return TransferType; }
	FileTransferStatus transferStatus() { return TransferStatus; }
	FileTransferError transferError() { return TransferError; }

	unsigned long fileSize() { return FileSize; }
	unsigned long transferredSize() { return TransferredSize; }

	void setLocalFileName(const QString &localFileName) { LocalFileName = localFileName; }
	QString localFileName() { return LocalFileName; }
	QString remoteFileName() { return RemoteFileName; }

	virtual void send() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void restore() = 0;

	virtual bool accept(const QFile &file);
	virtual void reject() = 0;

	unsigned int percent();

signals:
	void statusChanged();

};

#endif // FILE_TRANSFER_H
