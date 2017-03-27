/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "qfacebook-download-contacts-job.h"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/http/qfacebook-download-contacts-result.h"
#include "qfacebook/http/qfacebook-http-api.h"
#include "qfacebook/http/qfacebook-http-reply.h"
#include "qfacebook/http/qfacebook-http-request.h"
#include "qfacebook/session/qfacebook-session-token.h"
#include "qfacebook/qfacebook-contact.h"

#include <QtCore/QJsonArray>

QFacebookDownloadContactsJob::QFacebookDownloadContactsJob(QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QObject *parent) :
		QObject{parent},
		m_httpApi{httpApi},
		m_facebookSession{std::move(facebookSessionToken)}
{
	auto reply = m_httpApi.usersQuery(m_facebookSession.accessToken());
	connect(reply, &QFacebookHttpReply::finished, this, &QFacebookDownloadContactsJob::replyFinished);
}

QFacebookDownloadContactsJob::~QFacebookDownloadContactsJob()
{
}

void QFacebookDownloadContactsJob::replyFinished(const std::experimental::optional<QFacebookJsonReader> &result) try
{
	if (!result)
	{
		deleteLater();
		return;
	}

	auto messengerContacts = result->readObject("viewer").readObject("messenger_contacts");
	auto nodes = messengerContacts.readArray("nodes");
	auto friends = std::vector<QFacebookJsonReader>{};
	std::copy_if(std::begin(nodes), std::end(nodes), std::back_inserter(friends), [](const QFacebookJsonReader &v){
		return v.readObject("represented_profile").readString("friendship_status") == "ARE_FRIENDS";
	});

	std::transform(std::begin(friends), std::end(friends), std::back_inserter(m_contacts), [](const QFacebookJsonReader &v){
		return QFacebookContact::fromJson(v);
	});

	auto pageInfo = messengerContacts.readObject("page_info");
	if (!pageInfo.readBool("has_next_page"))
	{
		emit finished(QFacebookDownloadContactsResult{pageInfo.readString("delta_cursor").toUtf8(), m_contacts});
		deleteLater();
		return;
	}

	auto afterReply = m_httpApi.usersQueryAfter(m_facebookSession.accessToken(), pageInfo.readString("end_cursor").toUtf8());
	connect(afterReply, &QFacebookHttpReply::finished, this, &QFacebookDownloadContactsJob::replyFinished);
}
catch (QFacebookInvalidDataException &)
{
	deleteLater();
}
