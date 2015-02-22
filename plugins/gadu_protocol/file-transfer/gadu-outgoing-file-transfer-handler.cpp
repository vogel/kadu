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

#include "gadu-outgoing-file-transfer-handler.h"

#include "services/drive/gadu-drive-put-transfer.h"
#include "services/drive/gadu-drive-send-status-update-request.h"
#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-request.h"
#include "services/drive/gadu-drive-send-ticket-status.h"
#include "services/drive/gadu-drive-service.h"
#include "gadu-protocol.h"

#include "file-transfer/file-transfer-status.h"

#include <QtCore/QTimer>

GaduOutgoingFileTransferHandler::GaduOutgoingFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer) :
		OutgoingFileTransferHandler{fileTransfer},
		m_protocol{protocol},
		m_putStarted{false}
{
}

GaduOutgoingFileTransferHandler::~GaduOutgoingFileTransferHandler()
{
	clenaup();
}

void GaduOutgoingFileTransferHandler::clenaup()
{
	if (m_source)
	{
		m_source->close();
		m_source->deleteLater();
	}

	if (m_putTransfer)
		m_putTransfer->deleteLater();

	m_putStarted = false;
}

void GaduOutgoingFileTransferHandler::send(QIODevice *source)
{
	m_source = source;

	if (!m_protocol)
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		return;
	}

	auto contact = transfer().peer();
	auto driveService = m_protocol->driveService();
	auto sendTicketRequest = driveService->requestSendTicket(contact.id(), transfer().remoteFileName(), transfer().fileSize());
	connect(sendTicketRequest, SIGNAL(sendTickedReceived(GaduDriveSendTicket)), this, SLOT(statusUpdateReceived(GaduDriveSendTicket)));

	transfer().setTransferStatus(FileTransferStatus::WaitingForConnection);
}

void GaduOutgoingFileTransferHandler::statusUpdateReceived(GaduDriveSendTicket ticket)
{
	m_ticket = std::move(ticket);
	updateStatus();
}

void GaduOutgoingFileTransferHandler::updateStatus()
{
	if (!m_ticket.isValid())
	{
		transfer().setError(tr("Valid GG Drive ticket not available"));
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		clenaup();
		return;
	}

	transfer().setFileSize(m_ticket.fileSize());
	transfer().setTransferredSize(m_ticket.progress() * m_ticket.fileSize());

	if (m_ticket.status() == GaduDriveSendTicketStatus::Completed)
	{
		transfer().setTransferStatus(FileTransferStatus::Finished);
		transfer().setTransferredSize(m_ticket.fileSize());
		clenaup();
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

void GaduOutgoingFileTransferHandler::startOutgoingTransferIfNotStarted()
{
	if (m_putStarted)
		return;

	auto driveService = m_protocol->driveService();
	m_putStarted = true;
	m_putTransfer = driveService->putInOutbox(m_ticket, transfer().remoteFileName(), m_source);
}

void GaduOutgoingFileTransferHandler::requestSendStatusUpdate()
{
	auto driveService = m_protocol->driveService();
	auto updateSendStatusRequest = driveService->requestSendStatusUpdate(m_ticket.ticketId());

	connect(updateSendStatusRequest, SIGNAL(statusUpdateReceived(GaduDriveSendTicket)),
		this, SLOT(statusUpdateReceived(GaduDriveSendTicket)));
}

void GaduOutgoingFileTransferHandler::stop()
{
	if (m_putTransfer)
		m_putTransfer->deleteLater();

	transfer().setTransferStatus(FileTransferStatus::NotConnected);
	clenaup();
}

#include "moc_gadu-outgoing-file-transfer-handler.cpp"
