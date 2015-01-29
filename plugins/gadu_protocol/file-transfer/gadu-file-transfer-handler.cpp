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

#include <QtCore/QFileInfo>

#include "accounts/account.h"
#include "helpers/gadu-protocol-helper.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-request.h"
#include "services/drive/gadu-drive-service.h"

#include "gadu-file-transfer-handler.h"

GaduFileTransferHandler::GaduFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer) :
		FileTransferHandler(fileTransfer),
		CurrentProtocol{protocol}
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
		return;

	if (!CurrentProtocol)
		return;

	auto contact = transfer().peer();
	auto account = contact.contactAccount();
	transfer().setRemoteFileName(QString());

	if (account.isNull() || transfer().localFileName().isEmpty())
	{
		transfer().setTransferStatus(StatusNotConnected);
		deleteLater();
		return; // TODO: notify
	}

	auto driveService = CurrentProtocol->driveService();
	auto fileInfo = QFileInfo{transfer().localFileName()};
	auto sendTicketRequest = driveService->requestSendTicket(contact.id(), fileInfo.baseName(), fileInfo.size());

	connect(sendTicketRequest, SIGNAL(sendTickedReceived(GaduDriveSendTicket)), this, SLOT(sendTickedReceived(GaduDriveSendTicket)));

	transfer().setTransferStatus(StatusWaitingForConnection);
}

void GaduFileTransferHandler::sendTickedReceived(GaduDriveSendTicket ticket)
{
	if (!ticket.isValid())
	{
		transfer().setTransferStatus(StatusNotConnected);
		return;
	}

	transfer().setTransferStatus(StatusTransfer);

	auto driveService = CurrentProtocol->driveService();
	if (ticket.ackStatus() == GaduDriveSendTicketAckStatus::Allowed)
	{
		auto putTransfer = driveService->putInOutbox(ticket, transfer().localFileName());
		(void)putTransfer;
	}
}

void GaduFileTransferHandler::stop()
{
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
