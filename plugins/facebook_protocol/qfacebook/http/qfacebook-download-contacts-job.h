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

#pragma once

#include "qfacebook/session/qfacebook-session-token.h"
#include "qfacebook/qfacebook-json-reader.h"

#include <QtCore/QObject>
#include <experimental/optional>

class QFacebookContact;
class QFacebookHttpApi;
struct QFacebookDownloadContactsResult;

class QFacebookDownloadContactsJob : public QObject
{
	Q_OBJECT

public:
	explicit QFacebookDownloadContactsJob(QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QObject *parent = nullptr);
	virtual ~QFacebookDownloadContactsJob();

signals:
	void finished(const QFacebookDownloadContactsResult &result);

private:
	QFacebookHttpApi &m_httpApi;
	QFacebookSessionToken m_facebookSession;
	std::vector<QFacebookContact> m_contacts;

	void replyFinished(const std::experimental::optional<QFacebookJsonReader> &result);

};
