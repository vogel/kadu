/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_MANAGER_H
#define FILE_TRANSFER_MANAGER_H

#include <qobject.h>

#include "dcc_handler.h"
#include "file_transfer.h"

class ChatWidget;

class FileTransferWindow;

class FileTransferManager : public QObject, DccHandler
{
	Q_OBJECT

	FileTransferWindow *fileTransferWindow;
	int toggleFileTransferWindowMenuId;

	QStringList selectFilesToSend();

	void needFileAccept(DccSocket* socket);
	void sendFile(const UserListElements users);

	QValueList<FileTransfer *> Transfers;

private slots:
	void userboxMenuPopup();
	void sendFile();
	void kaduKeyPressed(QKeyEvent* e);

	void chatCreated(ChatWidget *chat);
	void chatDestroying(ChatWidget *chat);

	void fileDropped(const UserGroup *group, const QString &);

	void toggleFileTransferWindow();
	void sendFileActionActivated(const UserGroup* users);

	void transferDestroyed(QObject *transfer);

public:
	FileTransferManager(QObject *parent = 0, const char *name = 0);
	virtual ~FileTransferManager();

	void acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume = false);
	void rejectFile(DccSocket *socket);

	void showFileTransferWindow();

	void readFromConfig();
	void writeToConfig();

	void addSocket(DccSocket *socket) {}
	void removeSocket(DccSocket *socket) {}

	int dccType() { return 0; }

	bool socketEvent(DccSocket *socket, bool &lock);

	void connectionDone(DccSocket *socket) {}
	void connectionError(DccSocket *socket) {}

	void connectionAccepted(DccSocket *socket) {}
	void connectionRejected(DccSocket *socket) {}

	void addTransfer(FileTransfer *transfer);
	void removeTransfer(FileTransfer *transfer);

	const QValueList<FileTransfer *> transfers();

	FileTransfer * byUin(UinType);
	FileTransfer * byUinAndStatus(UinType, FileTransfer::FileTransferStatus);
	FileTransfer * search(FileTransfer::FileTransferType type, const UinType &contact, const QString &fileName,
		FileTransfer::FileNameType fileNameType = FileTransfer::FileNameFull);
	void destroyAll();

	void dcc7IncomingFileTransfer(DccSocket *socket);

private slots:
	void fileTransferFinishedSlot(FileTransfer *fileTransfer);
	void fileTransferWindowDestroyed();

public slots:
	/**
		Inicjuje wysy³anie pliku do podanego odbiorcy.
	**/
	void sendFile(UinType receiver);

	void sendFile(UinType receiver, const QString &filename);

signals:
	void newFileTransfer(FileTransfer *);
	void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
	void fileTransferStatusChanged(FileTransfer *);
	void fileTransferFinished(FileTransfer *);
	void fileTransferDestroying(FileTransfer *);

};

extern FileTransferManager* file_transfer_manager;

#endif
