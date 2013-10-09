/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QTextDocument>

#include "chat/type/chat-type-contact.h"
#include "file-transfer/file-transfer-manager.h"
#include "identities/identity.h"
#include "misc/misc.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
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

void NewFileTransferNotification::notifyIncomingFileTransfer(const FileTransfer &fileTransfer)
{
	Chat chat = ChatTypeContact::findChat(fileTransfer.peer(), ActionCreateAndAdd);
	NewFileTransferNotification *notification = new NewFileTransferNotification("FileTransfer/IncomingFile", fileTransfer,
			chat, fileTransfer.localFileName().isEmpty() ? StartNew : StartRestore);
	notification->setTitle(tr("Incoming transfer"));

	QString textFileSize = QString("%1 kB");
	double size = (double) fileTransfer.fileSize() / 1024.0;

	if (size > 1024.0)
	{
		size /= 1024.0;
		textFileSize = "%1 MB";
	}

	if (fileTransfer.localFileName().isEmpty())
		notification->setText(tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>Accept transfer?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name())));
	else
		notification->setText(tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>"
				"This is probably a next part of <b>%5</b><br/>What should I do?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()))
				.arg(Qt::escape(fileTransfer.localFileName())));

	NotificationManager::instance()->notify(notification);
}

NewFileTransferNotification::NewFileTransferNotification(const QString &type, FileTransfer ft, Chat chat, StartType startType) :
		ChatNotification(chat, type, KaduIcon()), ft(ft)
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

#include "moc_file-transfer-notifications.cpp"
