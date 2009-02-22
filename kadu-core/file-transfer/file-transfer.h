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
	Contact Peer;
	QString LocalFileName;
	FileTransferType TransferType;
	FileTransferStatus TransferStatus;
	FileTransferError TransferError;

	unsigned long FileSize;
	unsigned long TransferredSize;

protected:
	void changeFileTransferStatus(FileTransferStatus transferStatus);
	void changeFileTransferError(FileTransferError transferError);

	void setFileSize(unsigned long fileSize) { FileSize = fileSize; }
	void setTransferredSize(unsigned long transferredSize) { TransferredSize = transferredSize; }

	virtual void updateFileInfo() = 0;

public:
	FileTransfer(Contact peer, FileTransferType transferType);
	virtual ~FileTransfer();

	Contact contact() { return Peer; }

	FileTransferType transferType() { return TransferType; }
	FileTransferStatus transferStatus() { return TransferStatus; }
	FileTransferError transferError() { return TransferError; }

	void setLocalFileName(const QString &localFileName) { LocalFileName = localFileName; }
	QString localFileName() { return LocalFileName; }

	virtual void send() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void restore() = 0;

	unsigned int percent();

signals:
	void statusChanged();

};

#endif
