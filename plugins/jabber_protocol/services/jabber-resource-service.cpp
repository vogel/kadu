/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-resource-service.h"

#include "chat/chat.h"
#include "chat/chat-details-room.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"

JabberResourceService::JabberResourceService(QObject *parent) :
		QObject{parent}
{
}

JabberResourceService::~JabberResourceService()
{
}

void JabberResourceService::updateResource(JabberResource resource)
{
	auto it = std::find_if(std::begin(m_resources), std::end(m_resources), [&resource](const JabberResource &item) {
		return item.jid().full() == resource.jid().full();
	});

	if (it == std::end(m_resources))
		m_resources.append(resource);
	else
		*it = resource;
}

void JabberResourceService::removeResource(const Jid& jid)
{
	auto it = std::find_if(std::begin(m_resources), std::end(m_resources), [&jid](const JabberResource &item) {
		return item.jid().full() == jid.full();
	});

	if (it != std::end(m_resources))
		m_resources.erase(it);
}

void JabberResourceService::removeResources(const QString& bareJid)
{
	auto it = std::begin(m_resources);
	while (it != std::end(m_resources))
		if (it->jid().bare() == bareJid)
			it = m_resources.erase(it);
}

void JabberResourceService::clear()
{
	m_resources.clear();
}

JabberResource JabberResourceService::bestResource(const QString &bareJid) const
{
	auto result = JabberResource{};
	for (auto &&resource : m_resources)
		if (resource.jid().bare() == bareJid && resource.priority() > result.priority())
			result = resource;
	return result;
}

Jid JabberResourceService::bestChatJid(const Chat &chat) const
{
	if (!chat)
		return Jid{};

	auto chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return Jid{};

	if (chatType->name() == "Contact")
	{
		Q_ASSERT(1 == chat.contacts().size());
		return bestContactJid(chat.contacts().toContact());
	}

	if (chatType->name() == "Room")
	{
		auto details = qobject_cast<ChatDetailsRoom *>(chat.details());
		Q_ASSERT(details);

		return Jid::parse(details->room());
	}

	return Jid{};
}

Jid JabberResourceService::bestContactJid(const Contact &contact) const
{
	if (!contact)
		return Jid{};

	auto resource = contact.property("jabber:chat-resource", QString{}).toString();
	if (resource.isEmpty())
		resource = bestResource(contact.id()).jid().resource();
	return Jid::parse(contact.id()).withResource(resource);
}

#include "moc_jabber-resource-service.cpp"
