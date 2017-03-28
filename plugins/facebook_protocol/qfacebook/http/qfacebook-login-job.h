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

#include "libs/boost/variant/variant.hpp"
#include "qfacebook/qfacebook-json-reader.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <experimental/optional>

class QFacebookHttpApi;
class QFacebookSessionToken;

enum class QFacebookLoginErrorType
{
    Unknown,
    InvalidPassword
};

struct QFacebookLoginError
{
    QFacebookLoginErrorType type;
    QString message;
};

using QFacebookLoginResult = boost::variant<QFacebookLoginError, QFacebookSessionToken>;

class QFacebookLoginJob : public QObject
{
    Q_OBJECT

public:
    explicit QFacebookLoginJob(
        QFacebookHttpApi &httpApi, QString userName, QString password, QObject *parent = nullptr);
    virtual ~QFacebookLoginJob();

signals:
    void finished(const QFacebookLoginResult &result);

private:
    void replyFinished(const std::experimental::optional<QFacebookJsonReader> &result);
    QFacebookLoginResult loginResult(const std::experimental::optional<QFacebookJsonReader> &result) const;
};
