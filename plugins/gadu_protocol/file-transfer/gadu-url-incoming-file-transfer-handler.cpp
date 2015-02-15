/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-url-incoming-file-transfer-handler.h"

#include "services/drive/gadu-drive-get-transfer.h"
#include "services/drive/gadu-drive-service.h"
#include "gadu-protocol.h"

#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"

#include <QtNetwork/QNetworkReply>

GaduUrlIncomingFileTransferHandler::GaduUrlIncomingFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer) :
		UrlIncomingFileTransferHandler{fileTransfer},
		m_protocol{protocol}
{
	fileTransfer.setTransferType(FileTransferType::Url);
	if (fileTransfer.transferStatus() == FileTransferStatus::NotConnected)
		fileTransfer.setTransferStatus(FileTransferStatus::ReadyToDownload);
}

GaduUrlIncomingFileTransferHandler::~GaduUrlIncomingFileTransferHandler()
{
	clenaup();
}

void GaduUrlIncomingFileTransferHandler::clenaup()
{
	if (m_destination)
	{
		m_destination->close();
		m_destination->deleteLater();
	}

	if (m_getTransfer)
		m_getTransfer->deleteLater();
}

void GaduUrlIncomingFileTransferHandler::save(QIODevice *destination)
{
	clenaup();

	m_destination = destination;

	auto driveService = m_protocol->driveService();
	auto downloadId = transfer().property("gg:downloadId", QString{}).toString();
	auto remoteFileName = transfer().property("gg:remoteFileName", QString{}).toString();

	m_getTransfer = driveService->getFromDrive(downloadId, remoteFileName, m_destination);

	connect(m_getTransfer, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
	connect(m_getTransfer, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

	transfer().setTransferStatus(FileTransferStatus::Transfer);
	transfer().setTransferredSize(0);
}

void GaduUrlIncomingFileTransferHandler::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	transfer().setTransferredSize(bytesReceived);
	transfer().setFileSize(bytesTotal);
}

void GaduUrlIncomingFileTransferHandler::downloadFinished(QNetworkReply *reply)
{
	switch (reply->error())
	{
		case QNetworkReply::NoError:
			transfer().setTransferStatus(FileTransferStatus::Finished);
			break;

		default:
			transfer().setError(tr("Network error: %1").arg(reply->error()));
			break;
	}
}

#include "moc_gadu-url-incoming-file-transfer-handler.cpp"
