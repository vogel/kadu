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

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-type.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "identities/identity.h"
#include "notification/notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

FileTransferNotificationService::FileTransferNotificationService(QObject *parent) :
		QObject{parent},
		m_fileTransferAcceptCallback{QStringLiteral("file-transfer-accept"), tr("Accept"),
			[this](const Notification &notification){ return acceptFileTransfer(notification); }},
		m_fileTransferSaveCallback{QStringLiteral("file-transfer-save"), tr("Save"),
			[this](const Notification &notification){ return acceptFileTransfer(notification); }},
		m_fileTransferRejectCallback{QStringLiteral("file-transfer-reject"), tr("Reject"),
			[this](const Notification &notification){ return rejectFileTransfer(notification); }},
		m_fileTransferIgnoreCallback{QStringLiteral("file-transfer-ignore"), tr("Ignore"),
			[this](const Notification &notification){ return rejectFileTransfer(notification); }},
		m_fileTransferEvent{QStringLiteral("FileTransfer"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "File transfer"))},
		m_fileTransferIncomingEvent{QStringLiteral("FileTransfer/IncomingFile"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "Incoming file transfer"))}
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
	m_notificationEventRepository->addNotificationEvent(m_fileTransferEvent);
	m_notificationEventRepository->addNotificationEvent(m_fileTransferIncomingEvent);

	m_notificationCallbackRepository->addCallback(m_fileTransferAcceptCallback);
	m_notificationCallbackRepository->addCallback(m_fileTransferSaveCallback);
	m_notificationCallbackRepository->addCallback(m_fileTransferRejectCallback);
	m_notificationCallbackRepository->addCallback(m_fileTransferIgnoreCallback);
}

void FileTransferNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_fileTransferEvent);
	m_notificationEventRepository->removeNotificationEvent(m_fileTransferIncomingEvent);

	m_notificationCallbackRepository->removeCallback(m_fileTransferAcceptCallback);
	m_notificationCallbackRepository->removeCallback(m_fileTransferSaveCallback);
	m_notificationCallbackRepository->removeCallback(m_fileTransferRejectCallback);
	m_notificationCallbackRepository->removeCallback(m_fileTransferIgnoreCallback);
}

void FileTransferNotificationService::notifyIncomingFileTransfer(const FileTransfer &fileTransfer)
{
	auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, fileTransfer.peer(), ActionCreateAndAdd);

	auto data = QVariantMap{};
	data.insert(QStringLiteral("file-transfer"), qVariantFromValue(fileTransfer));
	data.insert(QStringLiteral("chat"), qVariantFromValue(chat));

	auto notification = Notification{};
	notification.type = QStringLiteral("FileTransfer/IncomingFile");
	notification.title = tr("Incoming transfer");
	notification.text = normalizeHtml(HtmlString{tr("<b>%1</b> wants to send you a file %2")}
			.arg(plainToHtml(fileTransfer.peer().display(true)), plainToHtml(fileTransfer.remoteFileName())));
	notification.details = incomingFileTransferDetails(chat, fileTransfer);
	notification.data = std::move(data);

	if (fileTransfer.transferType() == FileTransferType::Stream)
	{
		notification.callbacks.append(m_fileTransferAcceptCallback.name());
		notification.callbacks.append(m_fileTransferRejectCallback.name());
		notification.acceptCallback = m_fileTransferAcceptCallback.name();
		notification.discardCallback = m_fileTransferRejectCallback.name();
	}
	else
	{
		notification.callbacks.append(m_fileTransferSaveCallback.name());
		notification.callbacks.append(m_fileTransferIgnoreCallback.name());
		notification.acceptCallback = m_fileTransferSaveCallback.name();
		notification.discardCallback = m_fileTransferIgnoreCallback.name();
	}

	m_notificationService->notify(notification);
}

NormalizedHtmlString FileTransferNotificationService::incomingFileTransferDetails(const Chat &chat, const FileTransfer &fileTransfer)
{
	auto textFileSize = QStringLiteral("%1 kB");
	auto size = static_cast<double>(fileTransfer.fileSize()) / 1024.0;

	if (size > 1024.0)
	{
		size /= 1024.0;
		textFileSize = QStringLiteral("%1 MB");
	}

	if (fileTransfer.fileSize() > 0)
		return normalizeHtml(HtmlString{tr("size: <b>%1</b>, account: <b>%2</b>")}.arg(
			plainToHtml(textFileSize.arg(size, 0, 'f', 2)),
			plainToHtml(chat.chatAccount().accountIdentity().name())));
	else
		return normalizeHtml(HtmlString{tr("account: <b>%1</b>")}.arg(
			plainToHtml(chat.chatAccount().accountIdentity().name())));
}

void FileTransferNotificationService::acceptFileTransfer(const Notification &notification)
{
	auto fileTransfer = qvariant_cast<FileTransfer>(notification.data[QStringLiteral("file-transfer")]);
	m_fileTransferManager->acceptFileTransfer(fileTransfer, fileTransfer.localFileName());
}

void FileTransferNotificationService::rejectFileTransfer(const Notification &notification)
{
	auto fileTransfer = qvariant_cast<FileTransfer>(notification.data[QStringLiteral("file-transfer")]);
	m_fileTransferManager->rejectFileTransfer(fileTransfer);
}
