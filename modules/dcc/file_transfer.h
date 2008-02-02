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

#include <qdom.h>

#include "dcc.h"
#include "dcc_handler.h"
#include "protocol.h"

class DccSocket;
class FileTransferManager;

class FileTransfer : public QObject, public DccHandler
{
	Q_OBJECT

	friend class FileTransferManager;

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

	enum FileTransferError {
		ErrorDccDisabled,
		ErrorDccSocketTransfer,
		ErrorConnectionTimeout
	};

	enum StartType {
		StartNew,
		StartRestore
	};

	enum StopType {
		StopTemporary,
		StopFinally
	};

	enum FileNameType {
		FileNameFull,
		FileNameGadu
	};

private:
	QObject *mainListener;
	QValueList<QPair<QObject *, bool> > listeners;

	DccSocket *Socket;
	DccVersion Version;

	FileTransferType Type;
	FileTransferStatus Status;

	UinType Contact;
	QString FileName;
	QString GaduFileName;

	QTimer *connectionTimeoutTimer;
	QTimer *updateFileInfoTimer;

	long long int FileSize;
	long long int TransferedSize;
	long long int PrevTransferedSize;
	long int Speed;

	void connectSignals(QObject *, bool);
	void disconnectSignals(QObject *, bool);

	void startTimeout();
	void cancelTimeout();

	bool socketEvent(DccSocket *socket, bool &lock);

	void connectionDone(DccSocket *socket);
	void connectionError(DccSocket *socket);

	void setVersion();
	void prepareFileInfo();
	void startUpdateFileInfo();
	void stopUpdateFileInfo();

private slots:
	void connectionTimeout();
	void updateFileInfo();

public:
	FileTransfer(FileTransferManager *listener, DccVersion version, FileTransferType type, const UinType &contact,
		const QString &fileName);
	~FileTransfer();

	void addListener(QObject * const listener, bool listenerHasSlots);
	void removeListener(QObject * const listener, bool listenerHasSlots);

	bool addSocket(DccSocket *socket);
	void removeSocket(DccSocket *socket);

	int dccType();

	void start(StartType startType = StartNew);
	void stop(StopType stopType = StopTemporary);
	bool unused() { return Socket == 0; }

	void connectionAccepted(DccSocket *socket) {}
	void connectionRejected(DccSocket *socket);

	QDomElement toDomElement(const QDomElement &root);
	static FileTransfer * fromDomElement(const QDomElement &dom, FileTransferManager *listener);

	FileTransferType type();
	FileTransferStatus status();

	UinType contact();
	QString fileName();

	int percent();
	long int speed();
	long long int fileSize();
	long long int transferedSize();

signals:
	void newFileTransfer(FileTransfer *);
	void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
	void fileTransferStatusChanged(FileTransfer *);
	void fileTransferFinished(FileTransfer *);
	void fileTransferDestroying(FileTransfer *);
};

#endif
