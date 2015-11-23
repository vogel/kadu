/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer-notifications.h"

#include "chat/type/chat-type-contact.h"
#include "core/core.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-type.h"
#include "identities/identity.h"
#include "misc/misc.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-manager.h"
#include "notification/notification-event.h"

#include <QtGui/QTextDocument>

void NewFileTransferNotification::registerEvents()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent{"FileTransfer", QT_TRANSLATE_NOOP("@default", "File transfer")});
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent{"FileTransfer/IncomingFile", QT_TRANSLATE_NOOP("@default", "Incoming file transfer")});

	auto acceptTransferCallback = NotificationCallback{
		"file-transfer-accept",
		tr("Accept"),
		[](Notification *notification){
			auto fileTransferNotification = qobject_cast<NewFileTransferNotification *>(notification);
			if (fileTransferNotification)
				fileTransferNotification->callbackAccept();
		}
	};
	auto saveTransferCallback = NotificationCallback{
		"file-transfer-save",
		tr("Save"),
		[](Notification *notification){
			auto fileTransferNotification = qobject_cast<NewFileTransferNotification *>(notification);
			if (fileTransferNotification)
				fileTransferNotification->callbackAccept();
		}
	};
	auto rejectTransferCallback = NotificationCallback{
		"file-transfer-reject",
		tr("Reject"),
		[](Notification *notification){
			auto fileTransferNotification = qobject_cast<NewFileTransferNotification *>(notification);
			if (fileTransferNotification)
				fileTransferNotification->callbackReject();
		}
	};
	auto ignoreTransferCallback = NotificationCallback{
		"file-transfer-ignore",
		tr("Ignore"),
		[](Notification *notification){
			auto fileTransferNotification = qobject_cast<NewFileTransferNotification *>(notification);
			if (fileTransferNotification)
				fileTransferNotification->callbackReject();
		}
	};
	Core::instance()->notificationCallbackRepository()->addCallback(acceptTransferCallback);
	Core::instance()->notificationCallbackRepository()->addCallback(saveTransferCallback);
	Core::instance()->notificationCallbackRepository()->addCallback(rejectTransferCallback);
	Core::instance()->notificationCallbackRepository()->addCallback(ignoreTransferCallback);
}

void NewFileTransferNotification::unregisterEvents()
{
	if (Core::instance()) // TODO: hack
	{
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent{"FileTransfer", QT_TRANSLATE_NOOP("@default", "File transfer")});
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent{"FileTransfer/IncomingFile", QT_TRANSLATE_NOOP("@default", "Incoming file transfer")});
	}
}

void NewFileTransferNotification::notifyIncomingFileTransfer(const FileTransfer &fileTransfer)
{
	auto chat = ChatTypeContact::findChat(fileTransfer.peer(), ActionCreateAndAdd);
	auto notification = new NewFileTransferNotification{chat, "FileTransfer/IncomingFile", fileTransfer};
	notification->setTitle(tr("Incoming transfer"));

	auto textFileSize = QString("%1 kB");
	auto size = static_cast<double>(fileTransfer.fileSize()) / 1024.0;

	if (size > 1024.0)
	{
		size /= 1024.0;
		textFileSize = "%1 MB";
	}

	auto text = QString{};
	if (fileTransfer.fileSize() > 0)
		if (fileTransfer.localFileName().isEmpty())
			text = tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>Accept transfer?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			text = tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>"
				"This is probably a next part of <b>%5</b><br/>What should I do?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()))
				.arg(Qt::escape(fileTransfer.localFileName()));
	else
		if (fileTransfer.localFileName().isEmpty())
			text = tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>using account <b>%3</b>.<br/>Accept transfer?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			text = tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>using account <b>%3</b>.<br/>"
				"This is probably a next part of <b>%4</b><br/>What should I do?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()))
				.arg(Qt::escape(fileTransfer.localFileName()));

	notification->setText(text);

	Core::instance()->notificationManager()->notify(notification);
}

NewFileTransferNotification::NewFileTransferNotification(Chat chat, const QString &type, FileTransfer transfer) :
		Notification{Account::null, chat, type, KaduIcon{}},
		m_transfer{transfer}
{
	if (m_transfer.transferType() == FileTransferType::Stream)
	{
		addCallback("file-transfer-accept");
		addCallback("file-transfer-reject");
	}
	else
	{
		addCallback("file-transfer-save");
		addCallback("file-transfer-ignore");
	}
}

void NewFileTransferNotification::callbackAccept()
{
	close();

	Core::instance()->fileTransferManager()->acceptFileTransfer(m_transfer, m_transfer.localFileName());
}

void NewFileTransferNotification::callbackReject()
{
	close();

	Core::instance()->fileTransferManager()->rejectFileTransfer(m_transfer);
}

#include "moc_file-transfer-notifications.cpp"
