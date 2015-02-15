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

#include "gadu-drive-authorization.h"

#include "services/drive/gadu-drive-session-token.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDriveAuthorization::GaduDriveAuthorization(QString accountId, QString imToken, QString clientName, QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent},
		m_accountId{accountId},
		m_imToken{imToken},
		m_clientName{clientName},
		m_networkAccessManager{networkAccessManager}
{
}

GaduDriveAuthorization::~GaduDriveAuthorization()
{
	if (m_reply)
		m_reply->deleteLater();
}

void GaduDriveAuthorization::authorize()
{
	if (m_reply != nullptr)
		return;

	auto metadata = QJsonObject{};
	metadata["id"] = "01234567890123456789012345678901";
	metadata["name"] = "libgadu";
	metadata["os_version"] = "WINNT x86-msvc";
	metadata["client_version"] = m_clientName;
	metadata["type"] = "desktop";

	QNetworkRequest request;
	request.setUrl(QUrl{"https://drive.mpa.gg.pl/signin"});
	request.setRawHeader("Authorization", QString{"IMToken %1"}.arg(m_imToken).toAscii());
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("X-gged-api-version", "6");
	request.setRawHeader("X-gged-user", QString{"gg/pl:%1"}.arg(m_accountId).toAscii());
	request.setRawHeader("X-gged-client-metadata", QJsonDocument{metadata}.toJson(QJsonDocument::Compact).data());

	m_reply = m_networkAccessManager->put(request, QByteArray{});
	connect(m_reply.get(), SIGNAL(finished()), this, SLOT(requestFinished()));
}

void GaduDriveAuthorization::requestFinished()
{
	if (QNetworkReply::NoError == m_reply->error())
	{
		auto json = QJsonDocument::fromJson(m_reply->readAll());
		auto sessionData = json.object().value("result").toObject().value("session_data").toObject();
		auto sessionId = sessionData.value("session_id").toString();
		auto securityToken = sessionData.value("security_token").toString();
		auto token = GaduDriveSessionToken{std::move(sessionId), std::move(securityToken)};
		emit authorized(token);
	}
	else
		emit authorized({});

	deleteLater();
}

#include "moc_gadu-drive-authorization.cpp"
