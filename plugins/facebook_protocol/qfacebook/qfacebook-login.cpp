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

#include "qfacebook-login.h"
#include "qfacebook-login.moc"

#include "qfacebook/http/qfacebook-http-api.h"
#include "qfacebook/http/qfacebook-login-job.h"
#include "qfacebook/mqtt/qfacebook-mqtt-api.h"
#include "qfacebook/session/qfacebook-session.h"

#include "misc/memory.h"

#include <QtCore/QString>
#include <boost/variant/get.hpp>
#include <cassert>

QFacebookLogin::QFacebookLogin(
    QString userName, QString password, QFacebookDeviceId deviceId, Callback callback, QObject *parent)
        : QObject{parent}, m_httpApi{std::make_unique<QFacebookHttpApi>(deviceId.deviceId)},
          m_deviceId{std::move(deviceId)}, m_callback{std::move(callback)}
{
    assert(m_callback);

    auto loginJob = make_owned<QFacebookLoginJob>(*m_httpApi, std::move(userName), std::move(password), this);
    connect(loginJob, &QFacebookLoginJob::finished, this, &QFacebookLogin::loginJobFinished);
}

QFacebookLogin::~QFacebookLogin() = default;

void QFacebookLogin::loginJobFinished(const QFacebookLoginResult &result)
{
    auto error = boost::get<QFacebookLoginError>(&result);
    if (error)
    {
        m_callback(*error);
        return;
    }

    m_sessionToken = boost::get<QFacebookSessionToken>(result);
    m_mqttApi = std::make_unique<QFacebookMqttApi>(m_sessionToken, std::move(m_deviceId));
    connect(m_mqttApi.get(), &QFacebookMqttApi::connected, this, &QFacebookLogin::mqttConnected);
    connect(m_mqttApi.get(), &QFacebookMqttApi::disconnected, this, &QFacebookLogin::mqttDisconnected);
}

void QFacebookLogin::mqttConnected()
{
    disconnect(m_mqttApi.get(), &QFacebookMqttApi::connected, this, &QFacebookLogin::mqttConnected);
    disconnect(m_mqttApi.get(), &QFacebookMqttApi::disconnected, this, &QFacebookLogin::mqttDisconnected);

    auto session =
        std::make_unique<QFacebookSession>(std::move(m_sessionToken), std::move(m_httpApi), std::move(m_mqttApi));
    m_callback(std::move(session));
}

void QFacebookLogin::mqttDisconnected()
{
    disconnect(m_mqttApi.get(), &QFacebookMqttApi::connected, this, &QFacebookLogin::mqttConnected);
    disconnect(m_mqttApi.get(), &QFacebookMqttApi::disconnected, this, &QFacebookLogin::mqttDisconnected);

    m_callback(QFacebookLoginError{QFacebookLoginErrorType::Unknown, {}});
}
