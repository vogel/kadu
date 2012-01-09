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
	Contact contact(sender());

	Q_ASSERT(contact);
	Q_ASSERT(Contacts.contains(contact));

	if (StateInitialized == State)
		updateContact(contact);
}

bool RosterService::canPerformLocalUpdate() const
{
	if (StateInitialized != State)
		return false;

	// we reset State on disconnected signal
	Q_ASSERT(protocol()->isConnected());

	return true;
}

void RosterService::setState(RosterState state)
{
	State = state;
}

bool RosterService::addContact(const Contact &contact)
{
	if (Contacts.contains(contact))
		return false;

	Contacts.append(contact);
	connect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
	connect(contact, SIGNAL(buddyUpdated()), this, SLOT(contactUpdated()));

	return true;
}

bool RosterService::removeContact(const Contact &contact)
{
	if (!Contacts.contains(contact))
		return false;

	disconnect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
	disconnect(contact, SIGNAL(buddyUpdated()), this, SLOT(contactUpdated()));
	Contacts.removeAll(contact);

	return true;
}
