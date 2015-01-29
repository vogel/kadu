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

#include "gadu-drive-put-transfer.h"

#include "services/drive/gadu-drive-send-ticket.h"
#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDrivePutTransfer::GaduDrivePutTransfer(GaduDriveSessionToken sessionToken, GaduDriveSendTicket ticket, QString localFileName,
	QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent}
{
	m_file = new QFile{localFileName, this};
	if (!m_file->exists() || !m_file->open(QFile::ReadOnly))
		return;

	auto metadata = QJsonObject{};
	metadata["node_type"] = "file";

	auto url = QString{"https://drive.mpa.gg.pl/me/file/outbox/%1%%2C%2"}
		.arg(ticket.ticketId())
		.arg(QString::fromUtf8(QUrl::toPercentEncoding(QFileInfo{localFileName}.baseName())));

	QNetworkRequest request;
	request.setUrl(QUrl{url});
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("X-gged-api-version", "6");
	request.setRawHeader("X-gged-local-revision", "0");
	request.setRawHeader("X-gged-metadata", QJsonDocument{metadata}.toJson(QJsonDocument::Compact).data());
	request.setRawHeader("X-gged-security-token", sessionToken.securityToken().toAscii());

	printf("start transfer\n");

	m_reply = networkAccessManager->put(request, m_file.get());
	connect(m_reply.get(), SIGNAL(finished()), this, SLOT(requestFinished()));
}

GaduDrivePutTransfer::~GaduDrivePutTransfer()
{
}

void GaduDrivePutTransfer::requestFinished()
{
	printf("finished sending: %s\n", m_reply->readAll().data());
	m_reply->deleteLater();
	deleteLater();
}
