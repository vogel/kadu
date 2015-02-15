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

#include "gadu-drive-put-transfer.h"

#include "services/drive/gadu-drive-send-ticket.h"
#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDrivePutTransfer::GaduDrivePutTransfer(GaduDriveSessionToken sessionToken, GaduDriveSendTicket ticket, QString fileName, QIODevice *source,
	QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent}
{
	auto metadata = QJsonObject{};
	metadata["node_type"] = "file";

	// sigh, %2C inside string forbits me from using .arg().arg()
	auto url = QString{"https://drive.mpa.gg.pl/me/file/outbox/%1%2%3"}
		.arg(ticket.ticketId(), "%2C", QString::fromUtf8(QUrl::toPercentEncoding(fileName)));

	QNetworkRequest request;
	request.setUrl(QUrl{url});
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("X-gged-api-version", "6");
	request.setRawHeader("X-gged-local-revision", "0");
	request.setRawHeader("X-gged-metadata", QJsonDocument{metadata}.toJson(QJsonDocument::Compact).data());
	request.setRawHeader("X-gged-security-token", sessionToken.securityToken().toAscii());

	m_reply = networkAccessManager->put(request, source);
	connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

GaduDrivePutTransfer::~GaduDrivePutTransfer()
{
	if (m_reply)
		m_reply->deleteLater();
}

void GaduDrivePutTransfer::requestFinished()
{
	emit finished();
	deleteLater();
}
