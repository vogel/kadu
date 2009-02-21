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
		StatusFrozen,
		StatusWaitForConnection,
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

public:
// 	FileTransfer(FileTransferType type, const Contact &contact, const QString &fileName);
// 	virtual ~FileTransfer();

	Contact contact() { return Contact::null; }

	QString localFileName() { return ""; }

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
// signals:
// 	void newFileTransfer(FileTransfer *);
// // 	void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
// 	void fileTransferStatusChanged(FileTransfer *);
// 	void fileTransferFinished(FileTransfer *);
// 	void fileTransferDestroying(FileTransfer *);
};

#endif
