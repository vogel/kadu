/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
	if (FileTransferNotifyEvent)
		return;

	FileTransferNotifyEvent = new NotifyEvent("FileTransfer", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "File transfer"));
	FileTransferIncomingFileNotifyEvent = new NotifyEvent("FileTransfer/IncomingFile", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "Incoming file transfer"));

	NotificationManager::instance()->registerNotifyEvent(FileTransferNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(FileTransferIncomingFileNotifyEvent);
}

void NewFileTransferNotification::unregisterEvents()
{
	if (!FileTransferNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(FileTransferNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(FileTransferIncomingFileNotifyEvent);

	delete FileTransferNotifyEvent;
	delete FileTransferIncomingFileNotifyEvent;
}

NewFileTransferNotification::NewFileTransferNotification(const QString &type, FileTransfer ft, Chat chat, StartType startType) :
		ChatNotification(chat, type, QString()), ft(ft)
{
	if (startType == StartRestore)
	{
		addCallback(tr("Continue"), SLOT(callbackAccept()), "callbackAccept()");
		addCallback(tr("Save file under new name"), SLOT(callbackAcceptAsNew()), "callbackAcceptAsNew()");
		addCallback(tr("Ignore transfer"), SLOT(callbackDiscard()), "callbackDiscard()");

		Continue = true;
	}
	else
	{
		addCallback(tr("Accept"), SLOT(callbackAccept()), "callbackAccept()");
		addCallback(tr("Reject"), SLOT(callbackReject()), "callbackReject()");

		Continue = false;
	}

	setDefaultCallback(30 * 60 * 1000, SLOT(callbackDiscard()));
}

void NewFileTransferNotification::callbackAcceptAsNew()
{
	kdebugf();

	close();

	// let user choose new file name
	ft.setLocalFileName(QString());
	FileTransferManager::instance()->acceptFileTransfer(ft);
}

void NewFileTransferNotification::callbackAccept()
{
	kdebugf();

	close();

	if (!Continue) // let user choose new file name
		ft.setLocalFileName(QString());

	FileTransferManager::instance()->acceptFileTransfer(ft);
}

void NewFileTransferNotification::callbackReject()
{
	kdebugf();

	close();

	FileTransferManager::instance()->rejectFileTransfer(ft);
}
