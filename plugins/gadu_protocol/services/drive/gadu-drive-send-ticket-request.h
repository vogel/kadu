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

#include "services/drive/gadu-drive-send-ticket.h"
#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class QNetworkAccessManager;
class QNetworkReply;

class GaduDriveSendTicketRequest : public QObject
{
	Q_OBJECT

public:
	explicit GaduDriveSendTicketRequest(QString recipient, QString fileName, qint64 fileSize,
		GaduDriveSessionToken sessionToken, QNetworkAccessManager *networkAccessManager, QObject *parent = nullptr);
	virtual ~GaduDriveSendTicketRequest();

public slots:
	void authorized(GaduDriveSessionToken sessionToken);

signals:
	void sendTickedReceived(GaduDriveSendTicket);

private:
	QString m_recipient;
	QString m_fileName;
	qint64 m_fileSize;
	GaduDriveSessionToken m_sessionToken;
	QNetworkAccessManager *m_networkAccessManager;
	QPointer<QNetworkReply> m_reply;

	void sendRequest();

private slots:
	void requestFinished();

};
