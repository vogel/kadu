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

#include "qfacebook-download-threads-job.h"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/session/qfacebook-session-token.h"
#include "qfacebook-download-threads-result.h"
#include "qfacebook-http-api.h"
#include "qfacebook-http-reply.h"
#include "qfacebook-http-request.h"

#include <QtCore/QJsonArray>

QFacebookDownloadThreadsJob::QFacebookDownloadThreadsJob(QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QObject *parent) :
		QObject{parent},
		m_httpApi{httpApi},
		m_facebookSession{std::move(facebookSessionToken)}
{
	auto reply = m_httpApi.threadListQuery(m_facebookSession.accessToken());
	connect(reply, &QFacebookHttpReply::finished, this, &QFacebookDownloadThreadsJob::replyFinished);
}

QFacebookDownloadThreadsJob::~QFacebookDownloadThreadsJob()
{
}

void QFacebookDownloadThreadsJob::replyFinished(const std::experimental::optional<QFacebookJsonReader> &result) try
{
	deleteLater();

	if (!result)
		return;

	auto messageThreads = result->readObject("viewer").readObject("message_threads");
	emit finished(QFacebookDownloadThreadsResult{
		messageThreads.readString("sync_sequence_id").toInt(),
		messageThreads.readInt("unread_count")
	});
}
catch (QFacebookInvalidDataException &)
{
}

