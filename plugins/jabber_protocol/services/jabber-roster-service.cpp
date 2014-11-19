/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <xmpp_tasks.h>

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "roster/roster-entry.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-service-tasks.h"
#include "roster/roster-task.h"
#include "roster/roster-task-type.h"
#include "debug.h"

#include "services/jabber-roster-state.h"
#include "services/jabber-subscription-service.h"
#include "jabber-protocol.h"

#include "jabber-roster-service.h"

namespace XMPP
{

QStringList JabberRosterService::buddyGroups(const Buddy &buddy)
{
	QStringList result;

	foreach (const Group &group, buddy.groups())
		result.append(group.name());

	return result;
}

const QString & JabberRosterService::itemDisplay(const XMPP::RosterItem &item)
{
	if (!item.name().isEmpty())
		return item.name();
	else
		return item.jid().bare();
}

JabberRosterService::JabberRosterService(Protocol *protocol, const QVector<Contact> &contacts, QObject *parent) :
		RosterService{protocol, std::move(contacts), parent},
		m_tasks{new RosterServiceTasks{this}},
		State{JabberRosterState::NonInitialized}
{
	connect(protocol, SIGNAL(disconnected(Account)), this, SLOT(disconnected()));

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

void JabberRosterService::connectToClient()
{
	if (!XmppClient)
		return;

	connect(XmppClient.data(), SIGNAL(rosterItemAdded(const RosterItem &)),
	        this, SLOT(remoteContactUpdated(const RosterItem &)));
	connect(XmppClient.data(), SIGNAL(rosterItemUpdated(const RosterItem &)),
	        this, SLOT(remoteContactUpdated(const RosterItem &)));
	connect(XmppClient.data(), SIGNAL(rosterItemRemoved(const RosterItem &)),
	        this, SLOT(remoteContactDeleted(const RosterItem &)));
	connect(XmppClient.data(), SIGNAL(rosterRequestFinished(bool, int, QString)),
	        this, SLOT(rosterRequestFinished(bool)));
}

void JabberRosterService::disconnectFromClient()
{
	if (XmppClient)
		disconnect(XmppClient.data(), 0, this, 0);
}

void JabberRosterService::setClient(Client *xmppClient)
{
	disconnectFromClient();
	XmppClient = xmppClient;
	connectToClient();
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

JT_Roster * JabberRosterService::createContactTask(const Contact &contact)
{
	if (!XmppClient)
		return 0;

	XMPP::JT_Roster *rosterTask = new XMPP::JT_Roster(XmppClient->rootTask());
	connect(rosterTask, SIGNAL(finished()), this, SLOT(rosterTaskFinished()));
	connect(rosterTask, SIGNAL(destroyed(QObject*)), this, SLOT(rosterTaskDeleted(QObject*)));

	ContactForTask.insert(rosterTask, contact);

	return rosterTask;
}

bool JabberRosterService::isIntrestedIn(const XMPP::RosterItem &item)
{
	int subscription = item.subscription().type();

	if (XMPP::Subscription::Both == subscription)
		return true;

	if (XMPP::Subscription::To == subscription)
		return true;

	if (XMPP::Subscription::None != subscription && XMPP::Subscription::From != subscription)
		return false;

	if (item.ask() == "subscribe")
		return true;

	return false;
}

void JabberRosterService::remoteContactUpdated(const XMPP::RosterItem &item)
{
	if (JabberRosterState::NonInitialized == state())
		return;

	/**
	 * Subscription types are: Both, From, To, Remove, None.
	 * Both:   Both sides have authed each other, each side
	 *         can see each other's presence
	 * From:   The other side can see us.
	 * To:     We can see the other side. (implies we are
	 *         authed)
	 * Remove: Other side revoked our subscription request.
	 *         Not to be handled here.
	 * None:   No subscription.
	 *
	 * Regardless of the subscription type, we have to add
	 * a roster item here.
	 */

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionCreateAndAdd);
	if (!contact || contact == account().accountContact())
		return;

	if (!canPerformRemoteUpdate(contact))
	{
		m_markedForDelete.remove(contact);
		return;
	}

	if (!isIntrestedIn(item))
	{
		contact.rosterEntry()->setSynchronized();
		return;
	}
	else
		m_markedForDelete.remove(contact);

	contact.rosterEntry()->setSynchronizingFromRemote();

	ensureContactHasBuddyWithDisplay(contact, itemDisplay(item));

	Buddy buddy = contact.ownerBuddy();
	BuddyManager::instance()->addItem(buddy);

	QSet<Group> groups;
	foreach (const QString &group, item.groups())
		groups << GroupManager::instance()->byName(group);
	buddy.setGroups(groups);

	contact.rosterEntry()->setSynchronized();

	addContact(contact);
}

void JabberRosterService::remoteContactDeleted(const XMPP::RosterItem &item)
{
	if (JabberRosterState::NonInitialized == state())
		return;

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);

	RosterTaskType rosterTaskType = m_tasks->taskType(contact.id());
	if (RosterTaskType::None == rosterTaskType || RosterTaskType::Delete == rosterTaskType)
	{
		contact.rosterEntry()->setSynchronizingFromRemote();
		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		contact.rosterEntry()->setSynchronized();

		RosterService::removeContact(contact);
	}
}

void JabberRosterService::rosterTaskFinished()
{
	XMPP::JT_Roster *rosterTask = qobject_cast<XMPP::JT_Roster *>(sender());
	if (!rosterTask)
		return;

	if (!ContactForTask.contains(rosterTask))
		return;

	Contact contact = ContactForTask.value(rosterTask);
	if (!contact || !contact.rosterEntry())
		return;

	if (rosterTask->success())
	{
		// we dont want marking just-added detached contact as synchronized
		if (contact.rosterEntry()->state() != RosterEntryState::Detached)
			contact.rosterEntry()->setSynchronized();
		return;
	}

	XMPP::Stanza::Error error;
	if (!error.fromCode(rosterTask->statusCode()) || XMPP::Stanza::Error::Cancel == error.type)
		contact.rosterEntry()->setDetached();
	else
		contact.rosterEntry()->setHasLocalChanges();
}

void JabberRosterService::rosterTaskDeleted(QObject* object)
{
	ContactForTask.remove(static_cast<XMPP::JT_Roster *>(object));
}

void JabberRosterService::markContactsForDeletion()
{
	for (auto &&contact : contacts())
	{
		if (contact == account().accountContact())
			continue;

		RosterEntry *rosterEntry = contact.rosterEntry();
		RosterTaskType rosterTaskType = m_tasks->taskType(contact.id());

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
	Q_ASSERT(XmppClient);

	setState(JabberRosterState::Initializing);
	markContactsForDeletion();

	XmppClient->rosterRequest();
}

void JabberRosterService::rosterRequestFinished(bool success)
{
	Q_ASSERT(JabberRosterState::Initializing == state());

	if (success)
		deleteMarkedContacts();

	setState(JabberRosterState::Initialized);

	emit rosterReady(success);

	executeAllTasks();
}

bool JabberRosterService::canPerformLocalUpdate() const
{
	if (!protocol())
		return false;

	if (!protocol()->isConnected() || (JabberRosterState::Initialized != State))
		return false;

	if (!XmppClient)
		return false;

	return true;
}

bool JabberRosterService::canPerformRemoteUpdate(const Contact &contact) const
{
	if (contact.isAnonymous())
		return true;

	if (!contact.rosterEntry()->canAcceptRemoteUpdate())
		return false;

	return !m_tasks->containsTask(contact.id());
}

void JabberRosterService::executeAllTasks()
{
	while (!m_tasks->isEmpty())
	{
		executeTask(m_tasks->dequeue());
	}
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

	auto rosterTask = createContactTask(contact);
	if (!rosterTask)
		return;

	switch (taskType)
	{
		case RosterTaskType::Add:
			contact.setIgnoreNextStatusChange(true);
			rosterTask->set(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
			break;

		case RosterTaskType::Delete:
			rosterTask->remove(contact.id());
			break;

		case RosterTaskType::Update:
			rosterTask->set(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
			break;

		default:
			delete rosterTask;
			return;
	}

	rosterTask->go(true);
}

}

#include "moc_jabber-roster-service.cpp"
