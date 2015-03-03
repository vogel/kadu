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

#include "jid.h"

Jid Jid::parse(QString jid)
{
	auto slashIndex = jid.indexOf("/");
	auto resource = slashIndex == -1
			? QString{}
			: jid.mid(slashIndex + 1);
	auto rest = slashIndex == -1
			? jid
			: jid.mid(0, slashIndex);
	auto atIndex = rest.indexOf("@");
	auto domain = atIndex == -1
			? rest
			: rest.mid(atIndex + 1);
	auto node = atIndex == -1
			? QString{}
			: rest.mid(0, atIndex);
	return Jid{node, domain, resource};
}

Jid::Jid()
{
}

Jid::Jid(QString node, QString domain, QString resource) :
		m_node{node},
		m_domain{domain},
		m_resource{resource}
{
	m_bare = m_node.isEmpty()
			? m_domain
			: m_node + "@" + m_domain;
	m_full = m_resource.isEmpty()
			? m_bare
			: m_bare + "/" + m_resource;
}

Jid Jid::withNode(QString node) const
{
	return Jid{node, m_domain, m_resource};
}

Jid Jid::withDomain(QString domain) const
{
	return Jid{m_node, domain, m_resource};
}

Jid Jid::withResource(QString resource) const
{
	return Jid{m_node, m_domain, resource};
}

bool Jid::isEmpty() const
{
	return m_full.isEmpty();
}

QString Jid::full() const
{
	return m_full;
}

QString Jid::bare() const
{
	return m_bare;
}

QString Jid::node() const
{
	return m_node;
}

QString Jid::domain() const
{
	return m_domain;
}

QString Jid::resource() const
{
	return m_resource;
}
