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
#include "services/drive/gadu-drive-put-transfer.h"
#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-request.h"
#include "services/drive/gadu-drive-service.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "accounts/account.h"

#include <QtCore/QFileInfo>

GaduFileTransferHandler::GaduFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer) :
		FileTransferHandler{fileTransfer},
		m_protocol{protocol}
{
}

GaduFileTransferHandler::~GaduFileTransferHandler()
{
}

void GaduFileTransferHandler::finished(bool ok)
{
	transfer().setTransferStatus(ok
			? StatusFinished
			: StatusNotConnected);
	deleteLater();
}

void GaduFileTransferHandler::send()
{
	if (TypeSend != transfer().transferType()) // maybe assert here?
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
	transfer().setRemoteFileName(QString{});

	auto driveService = m_protocol->driveService();
	auto fileInfo = QFileInfo{transfer().localFileName()};

	auto sendTicketRequest = driveService->requestSendTicket(contact.id(), fileInfo.baseName(), fileInfo.size());
	connect(sendTicketRequest, SIGNAL(sendTickedReceived(GaduDriveSendTicket)), this, SLOT(sendTickedReceived(GaduDriveSendTicket)));

	transfer().setTransferStatus(StatusWaitingForConnection);
}

void GaduFileTransferHandler::sendTickedReceived(GaduDriveSendTicket ticket)
{
	m_ticket = std::move(ticket);

	if (!m_ticket.isValid())
	{
		finished(false);
		return;
	}

	if (m_ticket.ackStatus() == GaduDriveSendTicketAckStatus::Allowed)
		startOutgoingTransfer();
	else
		transfer().setTransferStatus(StatusWaitingForAccept);
}

void GaduFileTransferHandler::startOutgoingTransfer()
{
	if (m_putTransfer)
		return;

	transfer().setTransferStatus(StatusTransfer);

	auto driveService = m_protocol->driveService();
	m_putTransfer = driveService->putInOutbox(m_ticket, transfer().localFileName());
}

void GaduFileTransferHandler::stop()
{
	if (m_putTransfer)
		m_putTransfer->deleteLater();

	transfer().setTransferStatus(StatusNotConnected);
	deleteLater();
}

bool GaduFileTransferHandler::accept(const QString &fileName, bool resumeTransfer)
{
	Q_UNUSED(resumeTransfer);

	transfer().accept(fileName);
	return false;
}

void GaduFileTransferHandler::reject()
{
	deleteLater();
}

#include "moc_gadu-file-transfer-handler.cpp"
