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

#include "jabber-contact-avatar-service.h"
#include "jabber-contact-avatar-service.moc"

#include "jid.h"

#include "contacts/contact-id.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRosterManager.h>

JabberContactAvatarService::JabberContactAvatarService(QXmppClient *client, Account account, QObject *parent)
        : ContactAvatarService{account, parent}, m_client{client}
{
    connect(
        &m_client->rosterManager(), &QXmppRosterManager::rosterReceived, this,
        &JabberContactAvatarService::rosterReceived);
    connect(m_client, &QXmppClient::presenceReceived, this, &JabberContactAvatarService::presenceReceived);
}

JabberContactAvatarService::~JabberContactAvatarService() = default;

void JabberContactAvatarService::downloadAvatar(const ContactId &contactId, const QByteArray &id)
{
    (void)contactId;
    (void)id;
}

void JabberContactAvatarService::rosterReceived()
{
    for (auto &&bareId : m_client->rosterManager().getRosterBareJids())
        for (auto &&presence : m_client->rosterManager().getAllPresencesForBareJid(bareId))
            presenceReceived(presence);
}

void JabberContactAvatarService::presenceReceived(const QXmppPresence &presence)
{
    auto jid = Jid::parse(presence.from());
    auto contactId = ContactId{jid.bare().toUtf8()};

    switch (presence.vCardUpdateType())
    {
    case QXmppPresence::VCardUpdateNoPhoto:
        emit avatarRemoved(contactId);
        break;
    case QXmppPresence::VCardUpdateValidPhoto:
        emit avatarAvailable(contactId, presence.photoHash());
        break;
    default:
        break;
    }
}
