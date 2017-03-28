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

#include "qfacebook/http/qfacebook-login-job.h"
#include "qfacebook/qfacebook-device-id.h"
#include "qfacebook/session/qfacebook-session-token.h"

#include "libs/boost/variant/variant.hpp"

#include <QtCore/QObject>
#include <functional>
#include <memory>

class QFacebookHttpApi;
class QFacebookMqttApi;
class QFacebookSession;
struct QFacebookLoginError;

class QFacebookLogin final : public QObject
{
    Q_OBJECT

public:
    using Result = boost::variant<QFacebookLoginError, std::unique_ptr<QFacebookSession>>;
    using Callback = std::function<void(Result &&)>;

    explicit QFacebookLogin(
        QString userName, QString password, QFacebookDeviceId deviceId, Callback callback, QObject *parent = nullptr);
    virtual ~QFacebookLogin();

private:
    QFacebookSessionToken m_sessionToken;
    std::unique_ptr<QFacebookHttpApi> m_httpApi;
    std::unique_ptr<QFacebookMqttApi> m_mqttApi;
    QFacebookDeviceId m_deviceId;
    Callback m_callback;

    void loginJobFinished(const QFacebookLoginResult &result);
    void mqttConnected();
    void mqttDisconnected();
};
