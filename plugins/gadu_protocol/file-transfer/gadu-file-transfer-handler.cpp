/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gadu-file-transfer-handler.h"

#include "helpers/gadu-protocol-helper.h"
#include "services/drive/gadu-drive-get-transfer.h"
#include "services/drive/gadu-drive-put-transfer.h"
#include "services/drive/gadu-drive-send-status-update-request.h"
#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-request.h"
#include "services/drive/gadu-drive-send-ticket-status.h"
#include "services/drive/gadu-drive-service.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "accounts/account.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-status.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

GaduFileTransferHandler::GaduFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer) :
		FileTransferHandler{fileTransfer},
		m_protocol{protocol},
		m_putFinished{false}
{
}

GaduFileTransferHandler::~GaduFileTransferHandler()
{
}

void GaduFileTransferHandler::finished(bool ok)
{
	transfer().setTransferStatus(ok
			? FileTransferStatus::Finished
			: FileTransferStatus::NotConnected);
	deleteLater();
}

void GaduFileTransferHandler::send()
{
	if (FileTransferDirection::Outgoing != transfer().transferDirection()) // maybe assert here?
	{
		finished(false);
		return;
	}

	if (!m_protocol || transfer().localFileName().isEmpty())
	{
		finished(false);
		return;
	}

	auto contact = transfer().peer();
	auto driveService = m_protocol->driveService();
	auto fileInfo = QFileInfo{transfer().localFileName()};

	auto sendTicketRequest = driveService->requestSendTicket(contact.id(), fileInfo.fileName(), fileInfo.size());
	connect(sendTicketRequest, SIGNAL(sendTickedReceived(GaduDriveSendTicket)), this, SLOT(statusUpdateReceived(GaduDriveSendTicket)));

	transfer().setFileSize(fileInfo.size());
	transfer().setRemoteFileName(QString{});
	transfer().setTransferStatus(FileTransferStatus::WaitingForConnection);
}

void GaduFileTransferHandler::statusUpdateReceived(GaduDriveSendTicket ticket)
{
	m_ticket = std::move(ticket);
	updateStatus();
}

void GaduFileTransferHandler::updateStatus()
{
	if (m_putFinished)
		return;

	if (!m_ticket.isValid())
	{
		transfer().setError(tr("Valid GG Drive ticket not available"));
		finished(false);
		return;
	}

	transfer().setFileSize(m_ticket.fileSize());
	transfer().setTransferredSize(m_ticket.progress() * m_ticket.fileSize());

	if (m_ticket.status() == GaduDriveSendTicketStatus::Completed)
	{
		transfer().setTransferStatus(FileTransferStatus::Finished);
		transfer().setTransferredSize(m_ticket.fileSize());
		finished(true);
		return;
	}

	if (m_ticket.ackStatus() != GaduDriveSendTicketAckStatus::Allowed)
	{
		transfer().setTransferStatus(FileTransferStatus::WaitingForAccept);
		transfer().setTransferredSize(0);
	}
	else
	{
		startOutgoingTransferIfNotStarted();
		transfer().setTransferStatus(FileTransferStatus::Transfer);
	}

	QTimer::singleShot(1000, this, SLOT(requestSendStatusUpdate()));
}

void GaduFileTransferHandler::startOutgoingTransferIfNotStarted()
{
	if (m_putTransfer)
		return;

	auto file = new QFile{transfer().localFileName()};
	if (!file->exists() || !file->open(QFile::ReadOnly))
	{
		transfer().setError(tr("Unable to open file"));
		file->deleteLater();
		finished(false);
		return;
	}

	auto driveService = m_protocol->driveService();
	m_putTransfer = driveService->putInOutbox(m_ticket, QFileInfo{file->fileName()}.fileName(), file);
	connect(m_putTransfer, SIGNAL(finished()), this, SLOT(putFinished()));
}

void GaduFileTransferHandler::putFinished()
{
	m_putFinished = true;
}

void GaduFileTransferHandler::requestSendStatusUpdate()
{
	auto driveService = m_protocol->driveService();
	auto updateSendStatusRequest = driveService->requestSendStatusUpdate(m_ticket.ticketId());

	connect(updateSendStatusRequest, SIGNAL(statusUpdateReceived(GaduDriveSendTicket)),
		this, SLOT(statusUpdateReceived(GaduDriveSendTicket)));
}

void GaduFileTransferHandler::stop()
{
	if (m_getTransfer)
		m_getTransfer->deleteLater();
	if (m_putTransfer)
		m_putTransfer->deleteLater();

	transfer().setTransferStatus(FileTransferStatus::NotConnected);
	deleteLater();
}

void GaduFileTransferHandler::accept(QIODevice *destination)
{
	if (m_getTransfer)
	{
		destination->close();
		destination->deleteLater();
	}

	auto driveService = m_protocol->driveService();
	auto downloadId = transfer().property("gg:downloadId", QString{}).toString();
	auto remoteFileName = transfer().property("gg:remoteFileName", QString{}).toString();

	m_getTransfer = driveService->getFromDrive(downloadId, remoteFileName, destination);

	connect(m_getTransfer, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
	connect(m_getTransfer, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

	transfer().setTransferStatus(FileTransferStatus::Transfer);
	transfer().setTransferredSize(0);
}

void GaduFileTransferHandler::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	transfer().setTransferredSize(bytesReceived);
	transfer().setFileSize(bytesTotal);
}

void GaduFileTransferHandler::downloadFinished(QNetworkReply *reply)
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

	deleteLater();
}

void GaduFileTransferHandler::reject()
{
	deleteLater();
}

#include "moc_gadu-file-transfer-handler.cpp"
