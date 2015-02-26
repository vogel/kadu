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

void JabberResourceService::clear()
{
	m_resources.clear();
}

JabberResource JabberResourceService::bestResource(const QString &bareJid)
{
	auto result = JabberResource{};
	for (auto &&resource : m_resources)
		if (resource.jid().bare() == bareJid && resource.priority() > result.priority())
			result = resource;
	return result;
}

#include "moc_jabber-resource-service.cpp"
