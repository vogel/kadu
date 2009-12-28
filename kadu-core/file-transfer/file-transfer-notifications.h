/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_NOTIFICATION_H
#define FILE_TRANSFER_NOTIFICATION_H

#include "file-transfer/file-transfer.h"
#include "notify/chat-notification.h"

class FileTransferManager;
class DccSocket;
class NotifyEvent;

class NewFileTransferNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *FileTransferNotifyEvent;
	static NotifyEvent *FileTransferIncomingFileNotifyEvent;

	bool Continue;

	FileTransfer ft;

	QString fileName;

private slots:
	void callbackAcceptAsNew();

public slots:
	virtual void callbackAccept();
	virtual void callbackReject();

public:
	static void registerEvents();
	static void unregisterEvents();

	NewFileTransferNotification(const QString &type, FileTransfer ft, Chat chat, StartType startType);

};

#endif // FILE_TRANSFER_NOTIFICATION_H
