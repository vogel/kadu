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

#include "facebook-protocol.h"
#include "facebook-protocol.moc"

#include "facebook-account-data.h"
#include "facebook-services.h"
#include "qfacebook/http/qfacebook-login-job.h"
#include "qfacebook/publish/qfacebook-publish-presence.h"
#include "qfacebook/session/qfacebook-session.h"

#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "plugin/plugin-injected-factory.h"
#include "status/status-container.h"

#include "libs/boost/variant/get.hpp"

FacebookProtocol::FacebookProtocol(Account account, ProtocolFactory *factory) : Protocol{account, factory}
{
}

FacebookProtocol::~FacebookProtocol()
{
    logout();
}

void FacebookProtocol::login()
{
    if (m_facebookLogin)
        return;

    auto accountData = FacebookAccountData{account()};
    m_facebookLogin = std::make_unique<QFacebookLogin>(
        account().id(), account().password(), accountData.deviceId(),
        [&](QFacebookLogin::Result &&result) { facebookLoginFinished(std::move(result)); });
}

void FacebookProtocol::facebookLoginFinished(QFacebookLogin::Result &&result)
{
    m_facebookLogin.reset();
    m_facebookServices.reset();

    auto error = boost::get<QFacebookLoginError>(&result);
    if (error)
    {
        switch (error->type)
        {
        case QFacebookLoginErrorType::InvalidPassword:
            passwordRequired();
            break;

        default:
            emit connectionError(account(), "facebook.com", error->message);
            connectionError();
            break;
        }

        return;
    }

    auto facebookSession = std::move(boost::get<std::unique_ptr<QFacebookSession>>(result));
    connect(facebookSession.get(), &QFacebookSession::disconnected, this, &FacebookProtocol::facebookDisconnected);
    connect(
        facebookSession.get(), &QFacebookSession::invalidDataReceived, this, &FacebookProtocol::invalidDataReceived);
    connect(facebookSession.get(), &QFacebookSession::presenceReceived, this, &FacebookProtocol::presenceReceived);
    m_facebookServices = pluginInjectedFactory()->makeNotOwned<FacebookServices>(account(), std::move(facebookSession));

    loggedIn();
}

void FacebookProtocol::logout()
{
    m_facebookServices.release()->deleteLater();
    loggedOut();
}

void FacebookProtocol::facebookDisconnected()
{
    m_facebookServices.release()->deleteLater();
    connectionError();
}

void FacebookProtocol::sendStatusToServer()
{
    if (!isConnected() && !isDisconnecting())
        return;
}

QString FacebookProtocol::statusPixmapPath()
{
    return QStringLiteral("facebook");
}

void FacebookProtocol::invalidDataReceived(const QByteArray &data)
{
    (void)data;
}

void FacebookProtocol::presenceReceived(const QFacebookPublishPresence &presence)
{
    for (auto const &p : presence.presence)
    {
        auto newStatus = Status{};
        newStatus.setType(p.type == QFacebookPresenceType::Online ? StatusType::Online : StatusType::Offline);
        auto contact = contactManager()->byId(account(), QString::number(p.uid), ActionReturnNull);
        auto oldStatus = contact.currentStatus();
        contact.setCurrentStatus(newStatus);

        if (contact.ignoreNextStatusChange())
            contact.setIgnoreNextStatusChange(false);
        else
            emit contactStatusChanged(contact, oldStatus);
    }
}
