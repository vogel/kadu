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

#include "gadu-drive-send-status-update-request.h"

#include "services/drive/gadu-drive-send-ticket-parser.h"
#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDriveSendStatusUpdateRequest::GaduDriveSendStatusUpdateRequest(GaduDriveSessionToken sessionToken, QString ticketId,
	QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent},
		m_sessionToken{std::move(sessionToken)},
		m_ticketId{ticketId},
		m_networkAccessManager{networkAccessManager}
{
	if (!m_sessionToken.isValid())
	{
		emit statusUpdateReceived({});
		deleteLater();
	}
	else
		sendRequest();
}

GaduDriveSendStatusUpdateRequest::~GaduDriveSendStatusUpdateRequest()
{
	if (m_reply)
		m_reply->deleteLater();
}

void GaduDriveSendStatusUpdateRequest::sendRequest()
{
	if (m_reply != nullptr)
		return;

	QNetworkRequest request;
	request.setUrl(QUrl{QString{"https://drive.mpa.gg.pl/send_ticket/%1"}.arg(m_ticketId)});
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("X-gged-api-version", "6");
	request.setRawHeader("X-gged-security-token", m_sessionToken.securityToken().toAscii());

	m_reply = m_networkAccessManager->get(request);
	connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void GaduDriveSendStatusUpdateRequest::requestFinished()
{
	auto ticket = QNetworkReply::NoError == m_reply->error()
		? GaduDriveSendTicketParser::fromJson(QJsonDocument::fromJson(m_reply->readAll()))
		: GaduDriveSendTicket{};

	emit statusUpdateReceived(ticket);
	deleteLater();
}

#include "moc_gadu-drive-send-status-update-request.cpp"
