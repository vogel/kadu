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

#include "protocol.h"

class DccSocket;

class FileTransferManager;

class FileTransfer : public QObject
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
			StatusFinished
		};

		enum FileTransferError {
			ErrorDccDisabled,
			ErrorDccSocketTransfer,
			ErrorConnectionTimeout,
			ErrorDccTooManyConnections
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
		static QMap<DccSocket*, FileTransfer*> Transfers;

	public:
		static QValueList<FileTransfer *> AllTransfers;

	private:
		QObject *mainListener;
		QValueList<QPair<QObject *, bool> > listeners;

		DccSocket* Socket;
		FileTransferType Type;
		FileTransferStatus Status;

		UinType Contact;
		QString FileName;
		QString GaduFileName;

		QTimer* connectionTimeoutTimer;
		QTimer* updateFileInfoTimer;

		long long int FileSize;
		long long int TransferedSize;
		long long int PrevTransferedSize;
		long int Speed;

		bool dccFinished;
		bool direct;

		void connectSignals(QObject *, bool);
		void disconnectSignals(QObject *, bool);

		void socketDestroying();

	private slots:
		void connectionTimeout();

		void prepareFileInfo();
		void updateFileInfo();

	public:
		FileTransfer(FileTransferManager *listener, FileTransferType type, const UinType &contact,
			const QString &fileName);
		~FileTransfer();

		void addListener(QObject * const listener, bool listenerHasSlots);
		void removeListener(QObject * const listener, bool listenerHasSlots);

		void start(StartType startType = StartNew);
		void stop(StopType stopType = StopTemporary);

		void setSocket(DccSocket* Socket);

		static FileTransfer * bySocket(DccSocket* socket);
		static FileTransfer * byUin(UinType);
		static FileTransfer * byUinAndStatus(UinType, FileTransferStatus);
		static FileTransfer * search(FileTransferType type, const UinType &contact, const QString &fileName,
			FileNameType fileNameType = FileNameFull);
		static void destroyAll();

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

		void finished(bool successfull);

		void needFileInfo();
		void connectionBroken();
		void dccError();

	signals:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

#endif
