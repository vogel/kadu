/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

RosterService::RosterService(Account account, QObject *parent) :
		AccountService(account, parent), State(StateNonInitialized)
{
}

RosterService::~RosterService()
{
}

void RosterService::setProtocol(Protocol *protocol)
{
	if (CurrentProtocol)
		disconnect(CurrentProtocol.data(), 0, this, 0);

	CurrentProtocol = protocol;

	if (CurrentProtocol)
		connect(CurrentProtocol.data(), SIGNAL(disconnected(Account)), this, SLOT(disconnected()));
}

void RosterService::disconnected()
{
	setState(StateNonInitialized);
	setContacts(QVector<Contact>());
}

void RosterService::contactUpdated()
{
	Contact contact(sender());

	Q_ASSERT(contact);
	Q_ASSERT(Contacts.contains(contact));

	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	if (!contact.rosterEntry()->requiresSynchronization())
		return;

	addTask(RosterTask(RosterTaskUpdate, contact.id()));
	if (canPerformLocalUpdate())
		executeAllTasks();
}

bool RosterService::canPerformLocalUpdate() const
{
	if (!CurrentProtocol)
		return false;

	return CurrentProtocol->isConnected() && (StateInitializing != State && StateNonInitialized != State);
}

bool RosterService::canPerformRemoteUpdate(const Contact &contact) const
{
	if (contact.isAnonymous())
		return true;

	if (!contact.rosterEntry()->canAcceptRemoteUpdate())
		return false;

	return !IdToTask.contains(contact.id());
}

void RosterService::setState(RosterState state)
{
	State = state;
}

void RosterService::prepareRoster(const QVector<Contact> &contacts)
{
	setContacts(contacts);

	foreach (const Contact &contact, Contacts)
	{
		if (contact.rosterEntry() && RosterEntrySynchronizing == contact.rosterEntry()->state())
			contact.rosterEntry()->setState(RosterEntryDesynchronized);
		if (contact.rosterEntry() && contact.rosterEntry()->requiresSynchronization())
			addTask(RosterTask(RosterTaskUpdate, contact.id()));
	}
}

QVector<RosterTask> RosterService::tasks()
{
	return Tasks.toVector();
}

void RosterService::setTasks(const QVector<RosterTask> &tasks)
{
	Tasks.clear();
	IdToTask.clear();

	foreach (const RosterTask &task, tasks)
		addTask(task);
}

bool RosterService::shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType)
{
	Q_ASSERT(RosterTaskNone != taskType);
	Q_ASSERT(RosterTaskNone != replacementType);

	if (RosterTaskDelete == taskType)
		return true;

	if (RosterTaskAdd == taskType)
		return RosterTaskDelete == replacementType;

	return RosterTaskUpdate != replacementType;
}

void RosterService::setContacts(const QVector<Contact> &contacts)
{
	foreach (const Contact &contact, Contacts)
		disconnectContact(contact);

	Contacts = contacts;

	foreach (const Contact &contact, Contacts)
		connectContact(contact);
}

void RosterService::connectContact(const Contact &contact)
{
	connect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
}

void RosterService::disconnectContact(const Contact &contact)
{
	disconnect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
}

void RosterService::addTask(const RosterTask &task)
{
	if (!IdToTask.contains(task.id()))
	{
		Tasks.enqueue(task);
		return;
	}

	RosterTask existingTask = IdToTask.value(task.id());
	if (shouldReplaceTask(existingTask.type(), task.type()))
	{
		Tasks.removeAll(existingTask);
		IdToTask.remove(task.id());
		IdToTask.insert(task.id(), task);
		Tasks.enqueue(task);
	}
}

RosterTaskType RosterService::taskType(const QString &id)
{
	if (!IdToTask.contains(id))
		return RosterTaskNone;
	else
		return IdToTask.value(id).type();
}

void RosterService::executeAllTasks()
{
	while (!Tasks.isEmpty())
	{
		RosterTask task = Tasks.dequeue();
		IdToTask.remove(task.id());
		executeTask(task);
	}
}

void RosterService::addContact(const Contact &contact)
{
	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	if (Contacts.contains(contact))
		return;

	Contacts.append(contact);
	connectContact(contact);

	if (!contact.rosterEntry()->requiresSynchronization())
		return;

	addTask(RosterTask(RosterTaskAdd, contact.id()));
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void RosterService::removeContact(const Contact &contact)
{
	if (contact.contactAccount() != account())
		return;

	int index = Contacts.indexOf(contact);
	if (index < 0)
		return;

	Contacts.remove(index);
	disconnectContact(contact);

	if (!contact.rosterEntry()->requiresSynchronization())
		return;

	addTask(RosterTask(RosterTaskDelete, contact.id()));
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void RosterService::updateContact(const Contact& contact)
{
	if (contact.contactAccount() != account() || contact.isAnonymous())
		return;

	if (!contact.rosterEntry()->requiresSynchronization())
		return;

	addTask(RosterTask(RosterTaskUpdate, contact.id()));
	if (canPerformLocalUpdate())
		executeAllTasks();
}

#include "moc_roster-service.cpp"
