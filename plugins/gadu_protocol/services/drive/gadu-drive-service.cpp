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

#include "gadu-drive-service.h"

#include "services/drive/gadu-drive-authorization.h"
#include "services/drive/gadu-drive-get-transfer.h"
#include "services/drive/gadu-drive-put-transfer.h"
#include "services/drive/gadu-drive-send-status-update-request.h"
#include "services/drive/gadu-drive-send-ticket-request.h"

#include "services/gadu-imtoken-service.h"

#include "core/core.h"

#include <QtNetwork/QNetworkAccessManager>

GaduDriveService::GaduDriveService(Account account, QObject *parent) :
		AccountService{account, parent},
		m_networkAccessManager{new QNetworkAccessManager{this}}
{
}

GaduDriveService::~GaduDriveService()
{
}

void GaduDriveService::setGaduIMTokenService(GaduIMTokenService *imTokenService)
{
	m_imTokenService = imTokenService;
	connect(m_imTokenService, SIGNAL(imTokenChanged(QByteArray)), this, SLOT(imTokenChanged(QByteArray)));
}

GaduDriveSendTicketRequest * GaduDriveService::requestSendTicket(QString recipient, QString fileName, qint64 fileSize)
{
	auto sendTicketRequest = new GaduDriveSendTicketRequest{recipient, fileName, fileSize, m_sessionToken, m_networkAccessManager.get(), this};

	if (!m_sessionToken.isValid())
	{
		if (!m_authorization)
		{
			m_authorization = new GaduDriveAuthorization{account().id(), m_imTokenService->imToken(), Core::nameWithVersion(), m_networkAccessManager.get(), this};
			connect(m_authorization.get(), SIGNAL(authorized(GaduDriveSessionToken)), this, SLOT(authorized(GaduDriveSessionToken)));
			m_authorization->authorize();
		}

		connect(m_authorization.get(), SIGNAL(authorized(GaduDriveSessionToken)), sendTicketRequest, SLOT(authorized(GaduDriveSessionToken)));
	}

	return sendTicketRequest;
}

GaduDriveGetTransfer * GaduDriveService::getFromDrive(QString downloadId, QString fileName, QIODevice *destination)
{
	return new GaduDriveGetTransfer{downloadId, fileName, destination, m_networkAccessManager.get(), this};
}

GaduDrivePutTransfer * GaduDriveService::putInOutbox(GaduDriveSendTicket ticket, QString fileName, QIODevice *source)
{
	return new GaduDrivePutTransfer{m_sessionToken, ticket, fileName, source, m_networkAccessManager.get(), this};
}

GaduDriveSendStatusUpdateRequest * GaduDriveService::requestSendStatusUpdate(QString ticketId)
{
	return new GaduDriveSendStatusUpdateRequest(m_sessionToken, ticketId, m_networkAccessManager.get(), this);
}

void GaduDriveService::authorized(GaduDriveSessionToken sessionToken)
{
	m_sessionToken = sessionToken;
}

void GaduDriveService::imTokenChanged(QByteArray)
{
	m_sessionToken = {};
}

#include "moc_gadu-drive-service.cpp"
