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
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-type.h"
#include "identities/identity.h"
#include "notification/notification/notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

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

void FileTransferNotificationService::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
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

void FileTransferNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void FileTransferNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{QStringLiteral("FileTransfer"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "File transfer"))});
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{QStringLiteral("FileTransfer/IncomingFile"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Incoming file transfer"))});

	auto acceptTransferCallback = NotificationCallback{
		QStringLiteral("file-transfer-accept"),
		tr("Accept"),
		[this](Notification *notification){ return acceptFileTransfer(notification); }
	};
	auto saveTransferCallback = NotificationCallback{
		QStringLiteral("file-transfer-save"),
		tr("Save"),
		[this](Notification *notification){ return acceptFileTransfer(notification); }
	};
	auto rejectTransferCallback = NotificationCallback{
		QStringLiteral("file-transfer-reject"),
		tr("Reject"),
		[this](Notification *notification){ return rejectFileTransfer(notification); }
	};
	auto ignoreTransferCallback = NotificationCallback{
		QStringLiteral("file-transfer-ignore"),
		tr("Ignore"),
		[this](Notification *notification){ return rejectFileTransfer(notification); }
	};
	m_notificationCallbackRepository->addCallback(acceptTransferCallback);
	m_notificationCallbackRepository->addCallback(saveTransferCallback);
	m_notificationCallbackRepository->addCallback(rejectTransferCallback);
	m_notificationCallbackRepository->addCallback(ignoreTransferCallback);
}

void FileTransferNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{QStringLiteral("FileTransfer"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "File transfer"))});
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{QStringLiteral("FileTransfer/IncomingFile"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Incoming file transfer"))});

	m_notificationCallbackRepository->removeCallback(QStringLiteral("file-transfer-accept"));
	m_notificationCallbackRepository->removeCallback(QStringLiteral("file-transfer-save"));
	m_notificationCallbackRepository->removeCallback(QStringLiteral("file-transfer-reject"));
	m_notificationCallbackRepository->removeCallback(QStringLiteral("file-transfer-ignore"));
}

void FileTransferNotificationService::notifyIncomingFileTransfer(const FileTransfer &fileTransfer)
{
	auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, fileTransfer.peer(), ActionCreateAndAdd);

	auto data = QVariantMap{};
	data.insert(QStringLiteral("file-transfer"), fileTransfer);
	data.insert(QStringLiteral("chat"), chat);

	auto notification = m_injectedFactory->makeInjected<Notification>(data, QStringLiteral("FileTransfer/IncomingFile"), KaduIcon{});
	notification->setTitle(tr("Incoming transfer"));
	notification->setText(incomingFileTransferText(chat, fileTransfer));

	if (fileTransfer.transferType() == FileTransferType::Stream)
	{
		notification->addCallback(QStringLiteral("file-transfer-accept"));
		notification->addCallback(QStringLiteral("file-transfer-reject"));
		notification->setAcceptCallback(QStringLiteral("file-transfer-accept"));
		notification->setDiscardCallback(QStringLiteral("file-transfer-reject"));
	}
	else
	{
		notification->addCallback(QStringLiteral("file-transfer-save"));
		notification->addCallback(QStringLiteral("file-transfer-ignore"));
		notification->setAcceptCallback(QStringLiteral("file-transfer-save"));
		notification->setDiscardCallback(QStringLiteral("file-transfer-ignore"));
	}

	m_notificationService->notify(notification);
}

QString FileTransferNotificationService::incomingFileTransferText(const Chat &chat, const FileTransfer &fileTransfer)
{
	auto textFileSize = QStringLiteral("%1 kB");
	auto size = static_cast<double>(fileTransfer.fileSize()) / 1024.0;

	if (size > 1024.0)
	{
		size /= 1024.0;
		textFileSize = QStringLiteral("%1 MB");
	}

	if (fileTransfer.fileSize() > 0)
		if (fileTransfer.localFileName().isEmpty())
			return tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>Accept transfer?").arg(
				Qt::escape(fileTransfer.peer().display(true)),
				Qt::escape(fileTransfer.remoteFileName()),
				Qt::escape(textFileSize.arg(size, 0, 'f', 2)),
				Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			return tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>of size <b>%3</b> using account <b>%4</b>.<br/>"
				"This is probably a next part of <b>%5</b><br/>What should I do?").arg(
				Qt::escape(fileTransfer.peer().display(true)),
				Qt::escape(fileTransfer.remoteFileName()),
				Qt::escape(textFileSize.arg(size, 0, 'f', 2)),
				Qt::escape(chat.chatAccount().accountIdentity().name()),
				Qt::escape(fileTransfer.localFileName()));
	else
		if (fileTransfer.localFileName().isEmpty())
			return tr("User <b>%1</b> wants to send you a file <b>%2</b><br/>using account <b>%3</b>.<br/>Accept transfer?").arg(
				Qt::escape(fileTransfer.peer().display(true)),
				Qt::escape(fileTransfer.remoteFileName()),
				Qt::escape(chat.chatAccount().accountIdentity().name()));
		else
			return tr("User <b>%1</b> wants to send you a file <b/>%2</b><br/>using account <b>%3</b>.<br/>"
				"This is probably a next part of <b>%4</b><br/>What should I do?").arg(
				Qt::escape(fileTransfer.peer().display(true)),
				Qt::escape(fileTransfer.remoteFileName()),
				Qt::escape(chat.chatAccount().accountIdentity().name()),
				Qt::escape(fileTransfer.localFileName()));
}

void FileTransferNotificationService::acceptFileTransfer(Notification *notification)
{
	auto fileTransfer = qvariant_cast<FileTransfer>(notification->data()[QStringLiteral("file-transfer")]);
	notification->close();
	m_fileTransferManager->acceptFileTransfer(fileTransfer, fileTransfer.localFileName());
}

void FileTransferNotificationService::rejectFileTransfer(Notification *notification)
{
	auto fileTransfer = qvariant_cast<FileTransfer>(notification->data()[QStringLiteral("file-transfer")]);
	notification->close();
	m_fileTransferManager->rejectFileTransfer(fileTransfer);
}
