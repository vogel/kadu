/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-roster-service.h"

#include "services/jabber-roster-state.h"
#include "services/jabber-subscription-service.h"
#include "jabber-protocol.h"
#include "jid.h"

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "qxmpp/jabber-roster-extension.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"
#include "roster/roster-service-tasks.h"
#include "roster/roster-task-type.h"
#include "roster/roster-task.h"
#include "debug.h"

#include <qxmpp/QXmppRosterManager.h>

QSet<QString> JabberRosterService::buddyGroups(const Buddy &buddy)
{
	auto result = QSet<QString>{};
	for (auto &&group : buddy.groups())
		result.insert(group.name());
	return result;
}

JabberRosterService::JabberRosterService(QXmppRosterManager *roster, JabberRosterExtension *rosterExtension, const QVector<Contact> &contacts, Protocol *protocol) :
		RosterService{std::move(contacts), protocol},
		m_roster{roster},
		m_rosterExtension{rosterExtension},
		m_tasks{new RosterServiceTasks{this}},
		State{JabberRosterState::NonInitialized}
{
	connect(protocol, SIGNAL(disconnected(Account)), this, SLOT(disconnected()));

	connect(m_roster, SIGNAL(itemAdded(QString)), this, SLOT(remoteContactUpdated(QString)));
	connect(m_roster, SIGNAL(itemChanged(QString)), this, SLOT(remoteContactUpdated(QString)));
	connect(m_roster, SIGNAL(itemRemoved(QString)), this, SLOT(remoteContactDeleted(QString)));
	connect(m_roster, SIGNAL(rosterReceived()), this, SLOT(rosterRequestFinished()));
	connect(m_rosterExtension, SIGNAL(rosterCancelationReceived(Jid)), this, SLOT(rosterCancelationReceived(Jid)));

	connect(this, SIGNAL(contactAdded(Contact)), this, SLOT(contactAddedSlot(Contact)));
	connect(this, SIGNAL(contactRemoved(Contact)), this, SLOT(contactRemovedSlot(Contact)));
	connect(this, SIGNAL(contactUpdatedLocally(Contact)), this, SLOT(contactUpdatedSlot(Contact)));
}

JabberRosterService::~JabberRosterService()
{
}

RosterServiceTasks * JabberRosterService::tasks() const
{
    return m_tasks.get();
}

void JabberRosterService::disconnected()
{
	setState(JabberRosterState::NonInitialized);
}

void JabberRosterService::setState(JabberRosterState state)
{
	State = state;
}

void JabberRosterService::contactAddedSlot(Contact contact)
{
	m_tasks->addTask(RosterTask{RosterTaskType::Add, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void JabberRosterService::contactRemovedSlot(Contact contact)
{
	m_tasks->addTask(RosterTask{RosterTaskType::Delete, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void JabberRosterService::contactUpdatedSlot(Contact contact)
{
	m_tasks->addTask(RosterTask{RosterTaskType::Update, contact.id()});
	if (canPerformLocalUpdate())
		executeAllTasks();
}

void JabberRosterService::ensureContactHasBuddyWithDisplay(const Contact &contact, const QString &display)
{
	if (contact.isAnonymous()) // contact has anonymous buddy, we should search for other
	{
		contact.setOwnerBuddy(BuddyManager::instance()->byDisplay(display, ActionCreateAndAdd));
		contact.ownerBuddy().setAnonymous(false);
	}
	else
		contact.ownerBuddy().setDisplay(display);
}

void JabberRosterService::remoteContactUpdated(const QString &bareJid)
{
	if (JabberRosterState::NonInitialized == state())
		return;

	auto contact = ContactManager::instance()->byId(account(), bareJid, ActionCreateAndAdd);
	if (!contact || contact == account().accountContact())
		return;

	m_markedForDelete.remove(contact);
	if (!canPerformRemoteUpdate(contact))
		return;

	contact.rosterEntry()->setSynchronizingFromRemote();
	ensureContactHasBuddyWithDisplay(contact, itemDisplay(bareJid));

	auto buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	BuddyManager::instance()->addItem(buddy);

	auto item = m_roster->getRosterEntry(bareJid);

	auto groups = QSet<Group>{};
	for (auto &&group : item.groups())
		groups << GroupManager::instance()->byName(group);
	buddy.setGroups(groups);

	contact.rosterEntry()->setSynchronized();

	addContact(contact);
}

QString JabberRosterService::itemDisplay(const QString &bareJid)
{
	auto rosterEntry = m_roster->getRosterEntry(bareJid);
	if (rosterEntry.name().isEmpty())
		return bareJid;
	else
		return rosterEntry.name();
}

void JabberRosterService::remoteContactDeleted(const QString &bareJid)
{
	if (JabberRosterState::NonInitialized == state())
		return;

	auto contact = ContactManager::instance()->byId(account(), bareJid, ActionReturnNull);

	auto rosterTaskType = m_tasks->taskType(contact.id());
	if (RosterTaskType::None == rosterTaskType || RosterTaskType::Delete == rosterTaskType)
	{
		contact.rosterEntry()->setSynchronizingFromRemote();
		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		contact.rosterEntry()->setSynchronized();

		RosterService::removeContact(contact);
	}
}

void JabberRosterService::rosterCancelationReceived(const Jid &jid)
{
	auto contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);
	if (!contact)
		return;

	contact.rosterEntry()->setDetached();
}

void JabberRosterService::markContactsForDeletion()
{
	for (auto &&contact : contacts())
	{
		if (contact == account().accountContact())
			continue;

		auto rosterEntry = contact.rosterEntry();
		auto rosterTaskType = m_tasks->taskType(contact.id());

		if (rosterEntry && (RosterEntryState::Synchronized == rosterEntry->state())
				&& (RosterTaskType::None == rosterTaskType || RosterTaskType::Delete == rosterTaskType))
			m_markedForDelete.insert(contact);
	}
}

void JabberRosterService::deleteMarkedContacts()
{
	for (auto &&contact : m_markedForDelete)
	{
		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		contact.rosterEntry()->setSynchronized();
	}
}

void JabberRosterService::prepareRoster()
{
	fixupInitialState();
	m_tasks->addTasks(m_tasks->updateTasksForContacts(contacts()));

	Q_ASSERT(JabberRosterState::NonInitialized == state());
	// Q_ASSERT(XmppClient);

	setState(JabberRosterState::Initializing);
	markContactsForDeletion();

	// XmppClient->rosterRequest();
}

void JabberRosterService::rosterRequestFinished()
{
	if (JabberRosterState::Initializing != state())
		return;

	for (auto &&bareJid : m_roster->getRosterBareJids())
		remoteContactUpdated(bareJid);

	deleteMarkedContacts();
	setState(JabberRosterState::Initialized);

	emit rosterReady();

	executeAllTasks();
}

bool JabberRosterService::canPerformLocalUpdate() const
{
	if (!protocol())
		return false;

	if (!protocol()->isConnected() || (JabberRosterState::Initialized != State))
		return false;

	return true;
}

bool JabberRosterService::canPerformRemoteUpdate(const Contact &contact) const
{
	if (contact.isAnonymous())
		return true;
	if (contact.rosterEntry()->state() == RosterEntryState::SynchronizingToRemote)
		return true;
	if (contact.rosterEntry()->state() == RosterEntryState::Synchronized)
		return true;
	if (contact.rosterEntry()->state() == RosterEntryState::Unknown)
		return true;

	return false;
}

void JabberRosterService::executeAllTasks()
{
	while (!m_tasks->isEmpty())
		executeTask(m_tasks->dequeue());
}

void JabberRosterService::executeTask(const RosterTask& task)
{
	Q_ASSERT(JabberRosterState::Initialized == state());

	auto contact = ContactManager::instance()->byId(account(), task.id(), ActionReturnNull);
	auto taskType = contact ? task.type() : RosterTaskType::Delete;
	if (contact)
	{
		if ((taskType == RosterTaskType::Update) && !contact.rosterEntry()->requiresSynchronization())
			return;
		// we don't want marking just-added detached contacts as synchronizing
		if (contact.rosterEntry()->state() != RosterEntryState::Detached)
			contact.rosterEntry()->setSynchronizingToRemote();
	}

	switch (taskType)
	{
		case RosterTaskType::Add:
			contact.setIgnoreNextStatusChange(true);
			m_roster->addItem(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
			break;

		case RosterTaskType::Delete:
			m_roster->removeItem(contact.id());
			break;

		case RosterTaskType::Update:
			m_roster->addItem(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
			break;

		default:
			return;
	}
}

#include "moc_jabber-roster-service.cpp"
