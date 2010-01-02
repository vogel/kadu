/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <fcntl.h>

#include "file-transfer/file-transfer-manager.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "misc/misc.h"
#include "debug.h"

#include "file-transfer-notifications.h"

NotifyEvent * NewFileTransferNotification::FileTransferNotifyEvent = 0;
NotifyEvent * NewFileTransferNotification::FileTransferIncomingFileNotifyEvent = 0;

void NewFileTransferNotification::registerEvents()
{
	FileTransferNotifyEvent = new NotifyEvent("FileTransfer", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "File transfer"));
	FileTransferIncomingFileNotifyEvent = new NotifyEvent("FileTransfer/IncomingFile", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "Incoming file transfer"));

	NotificationManager::instance()->registerNotifyEvent(FileTransferNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(FileTransferIncomingFileNotifyEvent);
}

void NewFileTransferNotification::unregisterEvents()
{
	NotificationManager::instance()->unregisterNotifyEvent(FileTransferNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(FileTransferIncomingFileNotifyEvent);

	delete FileTransferNotifyEvent;
	delete FileTransferIncomingFileNotifyEvent;
}

NewFileTransferNotification::NewFileTransferNotification(const QString &type, FileTransfer ft, Chat chat, StartType startType) :
		ChatNotification(chat, type, QIcon()), ft(ft), fileName("")
{
	if (startType == StartRestore)
	{
		addCallback(tr("Continue"), SLOT(callbackAccept()));
		addCallback(tr("Save file under new name"), SLOT(callbackAcceptAsNew()));
		addCallback(tr("Ignore transfer"), SLOT(callbackDiscard()));

		Continue = true;
	}
	else
	{
		addCallback(tr("Accept"), SLOT(callbackAccept()));
		addCallback(tr("Reject"), SLOT(callbackReject()));

		Continue = false;
	}

	setDefaultCallback(30 * 60 * 1000, SLOT(callbackDiscard()));
}

void NewFileTransferNotification::callbackAcceptAsNew()
{
	kdebugf();

	close();
	if (Continue)
		FileTransferManager::instance()->acceptFileTransfer(ft, ft.localFileName());
	else // under new name = new file transfer
	{
		FileTransfer newTransfer = FileTransfer::create();
		newTransfer.setFileTransferAccount(ft.fileTransferAccount());
		newTransfer.setFileTransferContact(ft.fileTransferContact());
		newTransfer.setFileSize(ft.fileSize());
		newTransfer.setRemoteFileName(ft.remoteFileName());
		newTransfer.setTransferType(ft.transferType());
		FileTransferManager::instance()->addItem(newTransfer);
		FileTransferManager::instance()->acceptFileTransfer(newTransfer);
	}
}

void NewFileTransferNotification::callbackAccept()
{
	kdebugf();
	
	close();
	FileTransferManager::instance()->acceptFileTransfer(ft);
}

void NewFileTransferNotification::callbackReject()
{
	kdebugf();
	
	close();
	FileTransferManager::instance()->rejectFileTransfer(ft);
}
