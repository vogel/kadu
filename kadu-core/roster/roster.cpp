/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "roster.h"

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"
#include "roster/roster-service.h"

Roster * Roster::sm_instance = nullptr;

Roster * Roster::instance()
{
	if (!sm_instance)
		sm_instance = new Roster{};

	return sm_instance;
}

RosterService * Roster::rosterService(const Contact &contact)
{
	if (!contact)
		return nullptr;

	if (!contact.contactAccount())
		return nullptr;

	if (!contact.contactAccount().protocolHandler())
		return nullptr;

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
	if (!contact)
		return;

	auto service = rosterService(contact);
	if (service)
	{
		contact.rosterEntry()->setHasLocalChanges();
		service->addContact(contact);
	}
}

void Roster::removeContact(const Contact &contact) const
{
	if (!contact)
		return;

	auto service = rosterService(contact);
	if (service)
	{
		service->removeContact(contact);
	}
}

#include "moc_roster.cpp"
