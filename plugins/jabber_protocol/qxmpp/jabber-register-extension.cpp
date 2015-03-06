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

#include "jabber-register-extension.h"

#include <QtXml/QDomElement>
#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRegisterIq.h>

JabberRegisterExtension::JabberRegisterExtension()
{
}

JabberRegisterExtension::~JabberRegisterExtension()
{
}

void JabberRegisterExtension::sendRegisterIq(const QXmppRegisterIq &registerIq)
{
	if (client()->isConnected())
	{
		m_pendingIds.insert(registerIq.id());
		client()->sendPacket(registerIq);
	}
}

bool JabberRegisterExtension::handleStanza(const QDomElement &stanza)
{
	if (stanza.tagName() != "iq")
		return false;

	auto id = stanza.attribute("id");
	auto isRegisterIq = QXmppRegisterIq::isRegisterIq(stanza) || m_pendingIds.contains(id);

	if (!isRegisterIq)
		return false;

	m_pendingIds.remove(id);

	auto registerIq = QXmppRegisterIq{};
	registerIq.parse(stanza);
	emit registerIqReceived(registerIq);
	return true;
}
