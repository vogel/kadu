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

class Contact;

class FileTransfer : public QObject
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
		StatusTransfer,
		StatusFinished,
		StatusRejected
	};

	enum StartType {
		StartNew,
		StartRestore
	};

	enum StopType {
		StopTemporary,
		StopFinally
	};

private:
	Contact Peer;
	QString LocalFileName;
	FileTransferType TransferType;
	FileTransferStatus TransferStatus;

	unsigned long FileSize;
	unsigned long TransferredSize;

protected:
	void changeFileTransferStatus(FileTransferStatus transferStatus);

	void setFileSize(unsigned long fileSize) { FileSize = fileSize; }
	void setTransferredSize(unsigned long transferredSize) { TransferredSize = transferredSize; }

	virtual void updateFileInfo() = 0;

public:
	FileTransfer(Contact peer, FileTransferType transferType);
	virtual ~FileTransfer();

	Contact contact() { return Peer; }

	void setLocalFileName(const QString &localFileName) { LocalFileName = localFileName; }
	QString localFileName() { return LocalFileName; }

	virtual void send() = 0;

// 	virtual void start(StartType startType = StartNew) = 0;
// 	virtual void stop(StopType stopType = StopTemporary) = 0;

// 	void connectionAccepted(DccSocket *socket) {}
// 	void connectionRejected(DccSocket *socket);
// 
// 	QDomElement toDomElement(const QDomElement &root);
// 	static FileTransfer * fromDomElement(const QDomElement &dom, FileTransferManager *listener);
/*
	FileTransferType type();
	FileTransferStatus status();*/

// 	UinType contact();
// 	QString fileName();
// 
// 	int percent();
// 	long int speed();
// 	long long int fileSize();
// 	long long int transferedSize();
// 
signals:
	void statusChanged(FileTransferStatus status);
// 	void newFileTransfer(FileTransfer *);
// // 	void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
// 	void fileTransferStatusChanged(FileTransfer *);
// 	void fileTransferFinished(FileTransfer *);
// 	void fileTransferDestroying(FileTransfer *);
};

#endif
