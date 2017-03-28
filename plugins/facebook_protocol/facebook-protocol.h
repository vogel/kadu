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
#include "qfacebook/qfacebook-login.h"

#include "misc/memory.h"
#include "protocols/protocol.h"

class FacebookServices;
struct QFacebookPublishPresence;

class FacebookProtocol : public Protocol
{
    Q_OBJECT

public:
    explicit FacebookProtocol(Account account, ProtocolFactory *factory);
    virtual ~FacebookProtocol();

    virtual bool contactsListReadOnly() override
    {
        return true;
    }
    virtual QString statusPixmapPath() override;

protected:
    virtual void login() override;
    virtual void logout() override;
    virtual void sendStatusToServer() override;

private:
    std::unique_ptr<QFacebookLogin> m_facebookLogin;
    not_owned_qptr<FacebookServices> m_facebookServices;

    void facebookLoginFinished(QFacebookLogin::Result &&result);
    void facebookDisconnected();

    void invalidDataReceived(const QByteArray &data);
    void presenceReceived(const QFacebookPublishPresence &presence);
};
