/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Based on bitlbee-facebook by James Geboski (jgeboski@gmail.com) and
 * dequis <dx@dxzone.com.ar>.
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

#include "qfacebook/qfacebook-json-reader.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include <QtCore/QObject>
#include <experimental/optional>

class QFacebookHttpApi;
struct QFacebookDownloadThreadsResult;

class QFacebookDownloadThreadsJob : public QObject
{
    Q_OBJECT

public:
    explicit QFacebookDownloadThreadsJob(
        QFacebookHttpApi &httpApi, QFacebookSessionToken facebookSessionToken, QObject *parent = nullptr);
    virtual ~QFacebookDownloadThreadsJob();

signals:
    void finished(const QFacebookDownloadThreadsResult &result);

private:
    QFacebookHttpApi &m_httpApi;
    QFacebookSessionToken m_facebookSession;

    void replyFinished(const std::experimental::optional<QFacebookJsonReader> &result);
};
