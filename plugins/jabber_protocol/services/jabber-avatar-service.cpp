/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "services/jabber-avatar-downloader.h"
#include "services/jabber-avatar-uploader.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"
#include "jid.h"

#include "avatars/avatar-manager.h"
#include "contacts/contact-manager.h"

#include "jabber-avatar-service.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRosterManager.h>

JabberAvatarService::JabberAvatarService(QXmppClient *client, Account account, QObject *parent) :
		AvatarService{account, parent},
		m_client{client}
{
	connect(&m_client->rosterManager(), SIGNAL(rosterReceived()), this, SLOT(rosterReceived()));
	connect(m_client, SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(presenceReceived(QXmppPresence)));
}

JabberAvatarService::~JabberAvatarService()
{
}

void JabberAvatarService::setAvatarManager(AvatarManager *avatarManager)
{
	m_avatarManager = avatarManager;
}

void JabberAvatarService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberAvatarService::setVCardService(JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

AvatarDownloader * JabberAvatarService::createAvatarDownloader()
{
	auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
	if (!protocol->isConnected())
		return nullptr;
	return new JabberAvatarDownloader{VCardService.data(), this};
}

AvatarUploader * JabberAvatarService::createAvatarUploader()
{
	auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
	if (!protocol->isConnected())
		return nullptr;
	return new JabberAvatarUploader{VCardService.data(), this};
}

void JabberAvatarService::rosterReceived()
{
	for (auto &&bareId : m_client->rosterManager().getRosterBareJids())
		for (auto &&presence : m_client->rosterManager().getAllPresencesForBareJid(bareId))
			presenceReceived(presence);
}

void JabberAvatarService::presenceReceived(const QXmppPresence &presence)
{
	auto jid = Jid::parse(presence.from());
	auto contact = m_contactManager->byId(account(), jid.bare(), ActionReturnNull);
	if (!contact)
		return;

	switch (presence.vCardUpdateType())
	{
		case QXmppPresence::VCardUpdateNoPhoto:
			m_avatarManager->removeAvatar(contact);
			break;
		case QXmppPresence::VCardUpdateValidPhoto:
			m_avatarManager->updateAvatar(contact, true);
			break;
		default:
			break;
	}
}

#include "moc_jabber-avatar-service.cpp"
