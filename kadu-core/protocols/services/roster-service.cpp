/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "protocols/protocol.h"
#include "protocols/services/roster/roster-entry.h"

#include "roster-service.h"

RosterService::RosterService(Protocol *protocol) :
		ProtocolService(protocol), State(StateNonInitialized)
{
	Q_ASSERT(protocol);

	connect(protocol, SIGNAL(disconnected(Account)), this, SLOT(disconnected()));
}

RosterService::~RosterService()
{
}

void RosterService::disconnected()
{
	setState(StateNonInitialized);
}

void RosterService::contactUpdated()
{
	// TODO abstraction leak
	Q_ASSERT(sender()); // ChangeNotifier
	Q_ASSERT(sender()->parent()); // RosterEntry
	Q_ASSERT(sender()->parent()->parent()); // ContactShared

	Contact contact(sender()->parent()->parent());

	Q_ASSERT(contact);
	Q_ASSERT(Contacts.contains(contact));

	if (canPerformLocalUpdate())
		updateContact(contact);
}

bool RosterService::canPerformLocalUpdate() const
{
	return protocol()->isConnected() && (StateInitializing != State && StateNonInitialized != State);
}

void RosterService::setState(RosterState state)
{
	State = state;
}

void RosterService::addContact(const Contact &contact)
{
	if (!Contacts.contains(contact))
	{
		Contacts.append(contact);
		connect(contact.rosterEntry()->changeNotifier(), SIGNAL(changed()), this, SLOT(contactUpdated()));
	}
}

void RosterService::removeContact(const Contact &contact)
{
	if (Contacts.contains(contact))
	{
		disconnect(contact.rosterEntry()->changeNotifier(), SIGNAL(changed()), this, SLOT(contactUpdated()));
		Contacts.removeAll(contact);
	}
}
