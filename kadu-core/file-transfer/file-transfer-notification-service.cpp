/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer-notification-service.h"

#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "core/injected-factory.h"
#include "file-transfer/file-transfer-notifications.h"
#include "identities/identity.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-manager.h"

FileTransferNotificationService::FileTransferNotificationService(QObject *parent) :
		QObject{parent}
{
}

FileTransferNotificationService::~FileTransferNotificationService()
{
}

void FileTransferNotificationService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void FileTransferNotificationService::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void FileTransferNotificationService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void FileTransferNotificationService::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void FileTransferNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void FileTransferNotificationService::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void FileTransferNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"FileTransfer", QT_TRANSLATE_NOOP("@default", "File transfer")});
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"FileTransfer/IncomingFile", QT_TRANSLATE_NOOP("@default", "Incoming file transfer")});

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
	m_notificationCallbackRepository->addCallback(acceptTransferCallback);
	m_notificationCallbackRepository->addCallback(saveTransferCallback);
	m_notificationCallbackRepository->addCallback(rejectTransferCallback);
	m_notificationCallbackRepository->addCallback(ignoreTransferCallback);
}

void FileTransferNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"FileTransfer", QT_TRANSLATE_NOOP("@default", "File transfer")});
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"FileTransfer/IncomingFile", QT_TRANSLATE_NOOP("@default", "Incoming file transfer")});
}

void FileTransferNotificationService::notifyIncomingFileTransfer(const FileTransfer &fileTransfer)
{
	auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, fileTransfer.peer(), ActionCreateAndAdd);
	auto notification = m_injectedFactory->makeInjected<NewFileTransferNotification>(chat, "FileTransfer/IncomingFile", fileTransfer);
	notification->setTitle(tr("Incoming transfer"));
	notification->setText(incomingFileTransferText(chat, fileTransfer));

	m_notificationManager->notify(notification);
}

QString FileTransferNotificationService::incomingFileTransferText(const Chat &chat, const FileTransfer &fileTransfer)
{
	auto textFileSize = QString("%1 kB");
	auto size = static_cast<double>(fileTransfer.fileSize()) / 1024.0;

	if (size > 1024.0)
	{
		size /= 1024.0;
		textFileSize = "%1 MB";
	}

	if (fileTransfer.fileSize() > 0)
		if (fileTransfer.localFileName().isEmpty())
			return tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>Accept transfer?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			return tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>"
				"This is probably a next part of <b>%5</b><br/>What should I do?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(textFileSize.arg(size, 0, 'f', 2)))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()))
				.arg(Qt::escape(fileTransfer.localFileName()));
	else
		if (fileTransfer.localFileName().isEmpty())
			return tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>using account <b>%3</b>.<br/>Accept transfer?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			return tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>using account <b>%3</b>.<br/>"
				"This is probably a next part of <b>%4</b><br/>What should I do?")
				.arg(Qt::escape(fileTransfer.peer().display(true)))
				.arg(Qt::escape(fileTransfer.remoteFileName()))
				.arg(Qt::escape(chat.chatAccount().accountIdentity().name()))
				.arg(Qt::escape(fileTransfer.localFileName()));
}
