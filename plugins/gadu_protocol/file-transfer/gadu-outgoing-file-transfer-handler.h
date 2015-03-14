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

#pragma once

#include "file-transfer/outgoing-file-transfer-handler.h"

#include "services/drive/gadu-drive-send-ticket.h"

#include <QtCore/QPointer>

class GaduDrivePutTransfer;
class GaduProtocol;

class QNetworkReply;

class GaduOutgoingFileTransferHandler : public OutgoingFileTransferHandler
{
	Q_OBJECT

public:
	explicit GaduOutgoingFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer);
	virtual ~GaduOutgoingFileTransferHandler();

	virtual void send(QIODevice *source) override;
	virtual void stop() override;

private:
	QPointer<GaduProtocol> m_protocol;
	GaduDriveSendTicket m_ticket;
	QPointer<GaduDrivePutTransfer> m_putTransfer;
	QPointer<QIODevice> m_source;

	void clenaup();
	void startOutgoingTransferIfNotStarted();
	void updateStatus(bool initial);

private slots:
	void initialStatusUpdateReceived(GaduDriveSendTicket);
	void statusUpdateReceived(GaduDriveSendTicket);
	void requestSendStatusUpdate();

};
