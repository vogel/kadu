/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>
#include <QtGui/QPixmap>

class JabberVCardService;
class QXmppVCardIq;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAccountAvatarService
 * @short Service for downloading and uploading avatars for XMPP/Jabber protocol.
 * @see JabberAccountAvatarService
 *
 * See documentation of JabberAccountAvatarService to get general information about this service.
 *
 * JabberAccountAvatarService uses JabberVCardService to create AvatarDownloader and
 * AvatarUploader instances. Use setVCardService() to set this service.
 */
class JabberAccountAvatarService : public AccountAvatarService
{
    Q_OBJECT

public:
    /**
     * @short Create service instance.
     * @param account account of service
     * @param parent QObject parent of service
     */
    explicit JabberAccountAvatarService(Account account, QObject *parent = nullptr);
    virtual ~JabberAccountAvatarService();

    virtual void upload(const QPixmap &avatar) override;
    virtual bool canRemove() override
    {
        return true;
    }

    /**
     * @short Set VCard service object to use in this service.
     * @param vCardService VCard service object to use
     */
    void setVCardService(JabberVCardService *vCardService);

private:
    QPointer<JabberVCardService> m_vCardService;
    QPixmap m_avatar;

    void vCardDownloaded(bool ok, const QXmppVCardIq &vcard);
    void vCardUploaded(bool ok);
};

/**
 * @}
 */
