/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-drive-send-ticket-request.h"

#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-mode.h"
#include "services/drive/gadu-drive-send-ticket-status.h"
#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDriveSendTicketRequest::GaduDriveSendTicketRequest(QString recipient, QString fileName, qint64 fileSize,
	GaduDriveSessionToken sessionToken, QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent},
		m_recipient{recipient},
		m_fileName{fileName},
		m_fileSize{fileSize},
		m_sessionToken{std::move(sessionToken)},
		m_networkAccessManager{networkAccessManager}
{
	if (m_sessionToken.isValid())
		sendRequest();
}

GaduDriveSendTicketRequest::~GaduDriveSendTicketRequest()
{
}

void GaduDriveSendTicketRequest::authorized(GaduDriveSessionToken sessionToken)
{
	m_sessionToken = std::move(sessionToken);

	if (!m_sessionToken.isValid())
	{
		emit sendTickedReceived({});
		deleteLater();
	}
	else
		sendRequest();
}

void GaduDriveSendTicketRequest::sendRequest()
{
	if (m_reply != nullptr)
		return;

	auto sendTicket = QJsonObject{};
	sendTicket["recipient"] = m_recipient;
	sendTicket["file_name"] = m_fileName;
	sendTicket["file_size"] = QString::number(m_fileSize); // gg expects string

	auto requestContent = QJsonObject{};
	requestContent["send_ticket"] = sendTicket;

	QNetworkRequest request;
	request.setUrl(QUrl{"https://drive.mpa.gg.pl/send_ticket"});
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("X-gged-api-version", "6");
	request.setRawHeader("X-gged-security-token", m_sessionToken.securityToken().toAscii());

	m_reply = m_networkAccessManager->put(request, QJsonDocument{requestContent}.toJson());
	connect(m_reply.get(), SIGNAL(finished()), this, SLOT(requestFinished()));
}

void GaduDriveSendTicketRequest::requestFinished()
{
	if (QNetworkReply::NoError == m_reply->error())
	{
		auto json = QJsonDocument::fromJson(m_reply->readAll());
		auto sendTicket = json.object().value("result").toObject().value("send_ticket").toObject();
		auto tickedId = sendTicket.value("id").toString();
		auto sender = sendTicket.value("sender").toString();
		auto recipient = sendTicket.value("recipient").toString();
		auto fileName = sendTicket.value("file_name").toString();
		auto fileSize = sendTicket.value("file_size").toInt();
		auto progress = sendTicket.value("send_progress").toDouble();
		auto ackStatusString = sendTicket.value("ack_status").toString();
		auto ackStatus = ackStatusString == "allowed"
			? GaduDriveSendTicketAckStatus::Allowed
			: ackStatusString == "rejected"
			? GaduDriveSendTicketAckStatus::Rejected
			: GaduDriveSendTicketAckStatus::Unknown;
		auto modeString = sendTicket.value("send_mode").toString();
		auto mode = modeString == "publink"
			? GaduDriveSendTicketMode::Publink
			: GaduDriveSendTicketMode::Normal;
		auto statusString = sendTicket.value("send_status").toString();
		auto status = modeString == "complete"
			? GaduDriveSendTicketStatus::Complete
			: GaduDriveSendTicketStatus::InProgress;

		auto ticket = GaduDriveSendTicket{std::move(tickedId), std::move(sender), std::move(recipient), std::move(fileName),
			fileSize, progress, ackStatus, mode, status};
		emit sendTickedReceived(ticket);
	}
	else
		emit sendTickedReceived({});

	deleteLater();
}

#include "moc_gadu-drive-send-ticket-request.cpp"
