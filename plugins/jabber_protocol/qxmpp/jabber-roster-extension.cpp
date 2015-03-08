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

#include "jabber-roster-extension.h"

#include "services/jabber-error-service.h"
#include "jid.h"

#include <QtXml/QDomElement>
#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRosterIq.h>

JabberRosterExtension::JabberRosterExtension()
{
}

JabberRosterExtension::~JabberRosterExtension()
{
}

void JabberRosterExtension::setJabberErrorService(JabberErrorService *errorService)
{
	m_errorService = errorService;
}

bool JabberRosterExtension::handleStanza(const QDomElement &stanza)
{
	if (stanza.tagName() != "iq" || !QXmppRosterIq::isRosterIq(stanza))
		return false;

	auto rosterIq = QXmppRosterIq{};
	rosterIq.parse(stanza);

	if (!m_errorService->isErrorIq(rosterIq))
		return false;

	if (rosterIq.error().type() == QXmppStanza::Error::Cancel)
		for (auto &&item : rosterIq.items())
			emit rosterCancelationReceived(Jid::parse(item.bareJid()));

	return false;
}

#include "moc_jabber-roster-extension.cpp"
