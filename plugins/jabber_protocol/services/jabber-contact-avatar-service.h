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

#include "avatars/contact-avatar-service.h"

class QXmppClient;
class QXmppPresence;

class JabberContactAvatarService : public ContactAvatarService
{
    Q_OBJECT

public:
    explicit JabberContactAvatarService(QXmppClient *client, Account account, QObject *parent = nullptr);
    virtual ~JabberContactAvatarService();

    virtual void downloadAvatar(const ContactId &contactId, const QByteArray &id) override;

private:
    QXmppClient *m_client;

    void rosterReceived();
    void presenceReceived(const QXmppPresence &presence);
};
