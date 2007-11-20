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

#include "file_transfer.h"

class ChatWidget;

class FileTransferWindow;

class FileTransferManager : public QObject
{
	Q_OBJECT

	private:
		FileTransferWindow *fileTransferWindow;

		int toggleFileTransferWindowMenuId;

		QStringList selectFilesToSend();

		void needFileAccept(DccSocket* socket);
		void needFileInfo(DccSocket* socket);

		void sendFile(const UserListElements users);

	private slots:
		void userboxMenuPopup();
		void sendFile();
		void kaduKeyPressed(QKeyEvent* e);
		void connectionBroken(DccSocket* socket);

		void dccEvent(DccSocket *socket, bool &lock);
		void dccError(DccSocket* socket);

		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

		void chatCreated(ChatWidget *chat);
		void chatDestroying(ChatWidget *chat);

		void fileDropped(const UserGroup *group, const QString &);

		void toggleFileTransferWindow();
		void sendFileActionActivated(const UserGroup* users);

	public:
		FileTransferManager(QObject *parent = 0, const char *name = 0);
		virtual ~FileTransferManager();

		void acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume = false);
		void discardFile(DccSocket *socket);

		void showFileTransferWindow();

		void readFromConfig();
		void writeToConfig();

	private slots:
		void fileTransferFinishedSlot(FileTransfer *fileTransfer, bool ok);
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
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

extern FileTransferManager* file_transfer_manager;

#endif
