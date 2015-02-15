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

#include "services/drive/gadu-drive-session-token.h"

#include "misc/memory.h"
#include "protocols/services/account-service.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class GaduDriveAuthorization;
class GaduDriveGetTransfer;
class GaduDrivePutTransfer;
class GaduDriveSendStatusUpdateRequest;
class GaduDriveSendTicket;
class GaduDriveSendTicketRequest;
class GaduIMTokenService;

class QNetworkAccessManager;

class GaduDriveService : public AccountService
{
	Q_OBJECT

public:
	explicit GaduDriveService(Account account, QObject *parent = nullptr);
	virtual ~GaduDriveService();

	void setGaduIMTokenService(GaduIMTokenService *imTokenService);

	GaduDriveSendTicketRequest * requestSendTicket(QString recipient, QString fileName, qint64 fileSize);
	GaduDriveGetTransfer * getFromDrive(QString downloadId, QString fileName, QIODevice *destination);
	GaduDrivePutTransfer * putInOutbox(GaduDriveSendTicket ticket, QString fileName, QIODevice *source);
	GaduDriveSendStatusUpdateRequest * requestSendStatusUpdate(QString ticketId);

private:
	owned_qptr<QNetworkAccessManager> m_networkAccessManager;
	owned_qptr<GaduDriveAuthorization> m_authorization;
	QPointer<GaduIMTokenService> m_imTokenService;
	GaduDriveSessionToken m_sessionToken;

private slots:
	void authorized(GaduDriveSessionToken sessionToken);
	void imTokenChanged(QByteArray);

};
