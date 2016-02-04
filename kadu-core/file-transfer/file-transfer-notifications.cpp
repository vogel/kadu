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

#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-type.h"

NewFileTransferNotification::NewFileTransferNotification(Chat chat, const QString &type, FileTransfer transfer) :
		Notification{Account::null, chat, type, KaduIcon{}},
		m_transfer{transfer}
{
	if (m_transfer.transferType() == FileTransferType::Stream)
	{
		addCallback(QStringLiteral("file-transfer-accept"));
		addCallback(QStringLiteral("file-transfer-reject"));
	}
	else
	{
		addCallback(QStringLiteral("file-transfer-save"));
		addCallback(QStringLiteral("file-transfer-ignore"));
	}
}

void NewFileTransferNotification::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
}

void NewFileTransferNotification::callbackAccept()
{
	close();

	m_fileTransferManager->acceptFileTransfer(m_transfer, m_transfer.localFileName());
}

void NewFileTransferNotification::callbackReject()
{
	close();

	m_fileTransferManager->rejectFileTransfer(m_transfer);
}

#include "moc_file-transfer-notifications.cpp"
