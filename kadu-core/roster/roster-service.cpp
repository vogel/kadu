/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "roster-service.h"

#include "protocols/protocol.h"
#include "roster/roster-entry.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-service-tasks.h"
#include "roster/roster-task-type.h"

RosterService::RosterService(Protocol *protocol, const QVector<Contact> &contacts, QObject *parent) :
		ProtocolService{protocol, parent},
		m_tasks{new RosterServiceTasks{this}},
		m_contacts{std::move(contacts)}
{
	for (auto &&contact : m_contacts)
		connectContact(contact);
}

RosterService::~RosterService()
{
}

void RosterService::contactDirtinessChanged()
{
	auto contact = Contact{sender()};

	Q_ASSERT(contact);
	Q_ASSERT(m_contacts.contains(contact));

	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	emit contactUpdated(contact);

	if (!supportsTasks() || !contact.rosterEntry()->requiresSynchronization())
		return;

	m_tasks->addTask(RosterTask{RosterTaskType::Update, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

bool RosterService::canPerformLocalUpdate() const
{
	return false;
}

QVector<RosterTask> RosterService::updateTasksForContacts() const
{
	return RosterServiceTasks::updateTasksForContacts(m_contacts);
}

void RosterService::resetSynchronizingToDesynchronized()
{
	for (auto &&contact : m_contacts)
	{
		if (!contact.rosterEntry())
			continue;
		if (contact.rosterEntry()->state() == RosterEntryState::Synchronizing)
			contact.rosterEntry()->setState(RosterEntryState::Desynchronized);
	}
}

void RosterService::connectContact(const Contact &contact)
{
	connect(contact, SIGNAL(dirtinessChanged()), this, SLOT(contactDirtinessChanged()));
}

void RosterService::disconnectContact(const Contact &contact)
{
	disconnect(contact, SIGNAL(dirtinessChanged()), this, SLOT(contactDirtinessChanged()));
}

RosterServiceTasks * RosterService::tasks() const
{
	return m_tasks.get();
}

const QVector<Contact> & RosterService::contacts() const
{
	return m_contacts;
}

void RosterService::executeAllTasks()
{
	while (!m_tasks->isEmpty())
	{
		executeTask(m_tasks->dequeue());
	}
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

	if (!supportsTasks() || !contact.rosterEntry()->requiresSynchronization())
		return;

	m_tasks->addTask(RosterTask{RosterTaskType::Add, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void RosterService::addSynchronizedContact(const Contact& contact)
{
	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	if (m_contacts.contains(contact))
		return;

	contact.rosterEntry()->setState(RosterEntryState::Synchronized);
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

	emit contactRemoved(contact);

	m_contacts.remove(index);
	disconnectContact(contact);

	if (!supportsTasks() || !contact.rosterEntry()->requiresSynchronization())
		return;

	m_tasks->addTask(RosterTask{RosterTaskType::Delete, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

#include "moc_roster-service.cpp"
