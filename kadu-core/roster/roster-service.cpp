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

#include "roster-service.h"

#include "protocols/protocol.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"
#include "roster/roster-task-type.h"

RosterService::RosterService(const QVector<Contact> &contacts, Protocol *protocol) :
		ProtocolService{protocol, protocol},
		m_contacts{std::move(contacts)}
{
	for (auto &&contact : m_contacts)
		connectContact(contact);
}

RosterService::~RosterService()
{
}

void RosterService::contactUpdated()
{
	auto contact = Contact{sender()};

	Q_ASSERT(contact);
	Q_ASSERT(m_contacts.contains(contact));

	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	emit contactUpdated(contact);
}

void RosterService::contactUpdatedLocally()
{
	auto contact = Contact{sender()};

	Q_ASSERT(contact);
	Q_ASSERT(m_contacts.contains(contact));

	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	emit contactUpdatedLocally(contact);
}

void RosterService::fixupInitialState()
{
	for (auto &&contact : m_contacts)
		if (contact.rosterEntry())
			contact.rosterEntry()->fixupInitialState();
}

void RosterService::connectContact(const Contact &contact)
{
	connect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
	connect(contact, SIGNAL(updatedLocally()), this, SLOT(contactUpdatedLocally()));
}

void RosterService::disconnectContact(const Contact &contact)
{
	disconnect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
	disconnect(contact, SIGNAL(updatedLocally()), this, SLOT(contactUpdatedLocally()));
}

RosterServiceTasks * RosterService::tasks() const
{
	return nullptr;
}

const QVector<Contact> & RosterService::contacts() const
{
	return m_contacts;
}

void RosterService::addContact(const Contact &contact)
{
	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	if (m_contacts.contains(contact))
		return;

	m_contacts.append(contact);
	connectContact(contact);

	emit contactAdded(contact);
}

void RosterService::removeContact(const Contact &contact)
{
	if (contact.contactAccount() != account())
		return;

	int index = m_contacts.indexOf(contact);
	if (index < 0)
		return;

	m_contacts.remove(index);
	disconnectContact(contact);

	emit contactRemoved(contact);
}

#include "moc_roster-service.cpp"
