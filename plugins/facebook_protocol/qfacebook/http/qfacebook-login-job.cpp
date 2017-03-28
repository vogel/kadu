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

#include "qfacebook-login-job.h"

#include "qfacebook-http-api.h"
#include "qfacebook-http-reply.h"
#include "qfacebook-http-request.h"
#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qfacebook/session/qfacebook-session-token.h"

QFacebookLoginJob::QFacebookLoginJob(QFacebookHttpApi &httpApi, QString userName, QString password, QObject *parent)
        : QObject{parent}
{
    auto reply = httpApi.auth(std::move(userName), std::move(password));
    connect(reply, &QFacebookHttpReply::finished, this, &QFacebookLoginJob::replyFinished);
}

QFacebookLoginJob::~QFacebookLoginJob()
{
}

void QFacebookLoginJob::replyFinished(const std::experimental::optional<QFacebookJsonReader> &result)
{
    deleteLater();
    emit finished(loginResult(result));
}

QFacebookLoginResult
QFacebookLoginJob::loginResult(const std::experimental::optional<QFacebookJsonReader> &result) const try
{
    if (!result)
        return QFacebookLoginError{QFacebookLoginErrorType::Unknown, {}};

    if (!result->hasInt("error_code"))
        return QFacebookSessionToken::fromJson(*result);

    auto errorCode = result->readInt("error_code");
    auto errorMessage =
        result->hasObject("error_data") ? result->readObject("error_data").readString("error_message") : QString{};

    if (errorCode == 401)
        return QFacebookLoginError{QFacebookLoginErrorType::InvalidPassword, errorMessage};

    return QFacebookLoginError{QFacebookLoginErrorType::Unknown, errorMessage};
}
catch (QFacebookInvalidDataException &)
{
    return QFacebookLoginError{QFacebookLoginErrorType::Unknown, {}};
}
