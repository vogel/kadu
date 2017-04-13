/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatars/account-avatar-service.h"
#include "misc/memory.h"

#include <QtGui/QPixmap>

class OAuthManager;
class OAuthToken;

class QNetworkAccessManager;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAccountAvatarService
 * @short Service for downloading and uploading avatars for Gadu-Gadu protocol.
 * @see AccountAvatarService
 *
 * GaduAccountAvatarService does not require any protocol data or session as it uses HTTP communication channel instead
 * of Gadu Gadu one. Uploading avatar is done with OAuth authorization on http://avatars.nowe.gg/upload webservice.
 */
class GaduAccountAvatarService : public AccountAvatarService
{
    Q_OBJECT

public:
    /**
     * @short Create service instance.
     * @param account account of service
     * @param parent QObject parent of service
     */
    explicit GaduAccountAvatarService(Account account, QObject *parent = nullptr);

    virtual void upload(const QPixmap &avatar);
    virtual bool canRemove() override { return false; }

private:
    owned_qptr<OAuthManager> m_oauth;
    owned_qptr<QNetworkAccessManager> m_network;
    QPixmap m_avatar;

    void authorized(OAuthToken token);
    void transferFinished();
};

/**
 * @}
 */
