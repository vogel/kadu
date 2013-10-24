/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/services/roster/roster-service.h"

#include "roster.h"

Roster * Roster::Instance = 0;

Roster * Roster::instance()
{
	if (!Instance)
		Instance = new Roster();

	return Instance;
}

RosterService * Roster::rosterService(const Contact &contact)
{
	if (!contact)
		return 0;

	if (!contact.contactAccount())
		return 0;

	if (!contact.contactAccount().protocolHandler())
		return 0;

	return contact.contactAccount().protocolHandler()->rosterService();
}

Roster::Roster()
{
}

Roster::~Roster()
{
}

void Roster::addContact(const Contact &contact) const
{
	RosterService *service = rosterService(contact);
	if (service)
		service->addContact(contact);
}

void Roster::removeContact(const Contact &contact) const
{
	RosterService *service = rosterService(contact);
	if (service)
		service->removeContact(contact);
}

#include "moc_roster.cpp"
