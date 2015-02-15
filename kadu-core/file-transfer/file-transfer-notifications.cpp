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
#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include <QtGui/QTextDocument>

NotifyEvent * NewFileTransferNotification::m_fileTransferNotifyEvent = nullptr;
NotifyEvent * NewFileTransferNotification::m_fileTransferIncomingFileNotifyEvent = nullptr;

void NewFileTransferNotification::registerEvents()
{
	if (m_fileTransferNotifyEvent)
		return;

	m_fileTransferNotifyEvent = new NotifyEvent{"FileTransfer", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "File transfer")};
	m_fileTransferIncomingFileNotifyEvent = new NotifyEvent{"FileTransfer/IncomingFile", NotifyEvent::CallbackRequired,
			QT_TRANSLATE_NOOP("@default", "Incoming file transfer")};

	NotificationManager::instance()->registerNotifyEvent(m_fileTransferNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(m_fileTransferIncomingFileNotifyEvent);
}

void NewFileTransferNotification::unregisterEvents()
{
	if (!m_fileTransferNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(m_fileTransferNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(m_fileTransferIncomingFileNotifyEvent);

	delete m_fileTransferNotifyEvent;
	delete m_fileTransferIncomingFileNotifyEvent;
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

	NotificationManager::instance()->notify(notification);
}

NewFileTransferNotification::NewFileTransferNotification(Chat chat, const QString &type, FileTransfer transfer) :
		ChatNotification{chat, type, KaduIcon{}, false},
		m_transfer{transfer}
{
	if (m_transfer.transferType() == FileTransferType::Stream)
	{
		addCallback(tr("Accept"), SLOT(callbackAccept()), "callbackAccept()");
		addCallback(tr("Reject"), SLOT(callbackReject()), "callbackReject()");
	}
	else
	{
		addCallback(tr("Save"), SLOT(callbackAccept()), "callbackAccept()");
		addCallback(tr("Ignore"), SLOT(callbackReject()), "callbackReject()");
	}

	setDefaultCallback(30 * 60 * 1000, SLOT(callbackDiscard()));
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
