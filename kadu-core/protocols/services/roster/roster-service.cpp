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
#include "protocols/services/roster/roster-entry.h"
#include "protocols/services/roster/roster-state.h"
#include "protocols/services/roster/roster-task-type.h"

RosterService::RosterService(Account account, const QVector<Contact> &contacts, QObject *parent) :
		AccountService{account, parent},
		m_state{RosterState::NonInitialized},
		m_contacts{std::move(contacts)}
{
	for (auto &&contact : m_contacts)
		connectContact(contact);
}

RosterService::~RosterService()
{
}

void RosterService::setProtocol(Protocol *protocol)
{
	if (m_protocol)
		disconnect(m_protocol.data(), nullptr, this, nullptr);

	m_protocol = protocol;

	if (m_protocol)
		connect(m_protocol.data(), SIGNAL(disconnected(Account)), this, SLOT(disconnected()));
}

void RosterService::disconnected()
{
	setState(RosterState::NonInitialized);
}

void RosterService::contactDirtinessChanged()
{
	auto contact = Contact{sender()};

	Q_ASSERT(contact);
	Q_ASSERT(m_contacts.contains(contact));

	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	addTask(RosterTask{RosterTaskType::Update, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

bool RosterService::canPerformLocalUpdate() const
{
	if (!m_protocol)
		return false;

	return m_protocol->isConnected() && (RosterState::Initialized == m_state);
}

bool RosterService::canPerformRemoteUpdate(const Contact &contact) const
{
	if (contact.isAnonymous())
		return true;

	if (!contact.rosterEntry()->canAcceptRemoteUpdate())
		return false;

	return !m_idToTask.contains(contact.id());
}

void RosterService::setState(RosterState state)
{
	m_state = state;
}

QVector<RosterTask> RosterService::updateTasksForContacts(const QVector<Contact> &contacts)
{
	auto result = QVector<RosterTask>{};
	for (auto &&contact : contacts)
	{
		if (!contact.rosterEntry())
			continue;
		if (contact.rosterEntry()->requiresSynchronization())
			result.append(RosterTask{RosterTaskType::Update, contact.id()});
	}
	return result;
}

QVector<RosterTask> RosterService::updateTasksForContacts() const
{
	return updateTasksForContacts(m_contacts);
}

void RosterService::resetSynchronizingToDesynchronized()
{
	for (auto &&contact : m_contacts)
	{
		if (!contact.rosterEntry())
			continue;
		if (contact.rosterEntry()->state() == RosterEntrySynchronizing)
			contact.rosterEntry()->setState(RosterEntryDesynchronized);
	}
}

QVector<RosterTask> RosterService::tasks()
{
	return m_tasks.toVector();
}

bool RosterService::shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType)
{
	Q_ASSERT(RosterTaskType::None != taskType);
	Q_ASSERT(RosterTaskType::None != replacementType);

	if (RosterTaskType::Delete == taskType)
		return true;

	if (RosterTaskType::Add == taskType)
		return RosterTaskType::Delete == replacementType;

	return RosterTaskType::Update != replacementType;
}

void RosterService::connectContact(const Contact &contact)
{
	connect(contact, SIGNAL(dirtinessChanged()), this, SLOT(contactDirtinessChanged()));
}

void RosterService::disconnectContact(const Contact &contact)
{
	disconnect(contact, SIGNAL(dirtinessChanged()), this, SLOT(contactDirtinessChanged()));
}

void RosterService::addTask(const RosterTask &task)
{
	if (!m_idToTask.contains(task.id()))
	{
		m_tasks.enqueue(task);
		return;
	}

	auto existingTask = m_idToTask.value(task.id());
	if (shouldReplaceTask(existingTask.type(), task.type()))
	{
		m_tasks.removeAll(existingTask);
		m_idToTask.remove(task.id());
		m_idToTask.insert(task.id(), task);
		m_tasks.enqueue(task);
	}
}

void RosterService::addTasks(const QVector<RosterTask> &tasks)
{
	for (auto const &task : tasks)
		addTask(task);
}

RosterTaskType RosterService::taskType(const QString &id)
{
	if (!m_idToTask.contains(id))
		return RosterTaskType::None;
	else
		return m_idToTask.value(id).type();
}

void RosterService::executeAllTasks()
{
	while (!m_tasks.isEmpty())
	{
		RosterTask task = m_tasks.dequeue();
		m_idToTask.remove(task.id());
		executeTask(task);
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

	addTask(RosterTask{RosterTaskType::Add, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
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

	addTask(RosterTask{RosterTaskType::Delete, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void RosterService::updateContact(const Contact& contact)
{
	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	addTask(RosterTask{RosterTaskType::Update, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

#include "moc_roster-service.cpp"
