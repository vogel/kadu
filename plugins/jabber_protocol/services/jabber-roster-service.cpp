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
#include "protocols/services/roster/roster-entry.h"
#include "protocols/services/roster/roster-task.h"
#include "debug.h"

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

JabberRosterService::JabberRosterService(Account account, QObject *parent) :
		RosterService(account, parent)
{
}

JabberRosterService::~JabberRosterService()
{
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

	if (!item.name().isEmpty())
		return true;

	if (!item.groups().isEmpty())
		return true;

	return false;
}

void JabberRosterService::remoteContactUpdated(const XMPP::RosterItem &item)
{
	if (StateNonInitialized == state())
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

	contact.rosterEntry()->setRemotelyDeleted(false);
	if (!canPerformRemoteUpdate(contact))
		return;

	if (!isIntrestedIn(item))
	{
		contact.rosterEntry()->setState(RosterEntrySynchronized);
		return;
	}

	contact.rosterEntry()->setState(RosterEntrySynchronizing);

	ensureContactHasBuddyWithDisplay(contact, itemDisplay(item));

	Buddy buddy = contact.ownerBuddy();
	BuddyManager::instance()->addItem(buddy);

	RosterService::addContact(contact);

	QSet<Group> groups;
	foreach (const QString &group, item.groups())
		groups << GroupManager::instance()->byName(group);
	buddy.setGroups(groups);

	contact.rosterEntry()->setState(RosterEntrySynchronized);
}

void JabberRosterService::remoteContactDeleted(const XMPP::RosterItem &item)
{
	if (StateNonInitialized == state())
		return;

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);

	RosterTaskType rosterTaskType = taskType(contact.id());
	if (RosterTaskNone == rosterTaskType || RosterTaskDelete == rosterTaskType)
	{
		contact.rosterEntry()->setState(RosterEntrySynchronizing);
		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		contact.rosterEntry()->setState(RosterEntrySynchronized);

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
		contact.rosterEntry()->setState(RosterEntrySynchronized);
		return;
	}

	XMPP::Stanza::Error error;
	if (!error.fromCode(rosterTask->statusCode()) || XMPP::Stanza::Error::Cancel == error.type)
		contact.rosterEntry()->setDetached(true);

	contact.rosterEntry()->setState(RosterEntryDesynchronized);
}

void JabberRosterService::rosterTaskDeleted(QObject* object)
{
	ContactForTask.remove(static_cast<XMPP::JT_Roster *>(object));
}

void JabberRosterService::markContactsForDeletion()
{
	QVector<Contact> accountContacts = ContactManager::instance()->contacts(account());
	foreach (const Contact &contact, accountContacts)
	{
		if (contact == account().accountContact())
			continue;

		RosterEntry *rosterEntry = contact.rosterEntry();
		RosterTaskType rosterTaskType = taskType(contact.id());

		if (rosterEntry && (RosterEntrySynchronized == rosterEntry->state())
				&& (RosterTaskNone == rosterTaskType || RosterTaskDelete == rosterTaskType))
			rosterEntry->setRemotelyDeleted(true);
	}
}

void JabberRosterService::deleteMarkedContacts()
{
	QVector<Contact> accountContacts = ContactManager::instance()->contacts(account());
	foreach (const Contact &contact, accountContacts)
	{
		if (contact == account().accountContact())
			continue;

		RosterEntry *rosterEntry = contact.rosterEntry();
		if (!rosterEntry || !rosterEntry->remotelyDeleted())
			continue;

		BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		contact.rosterEntry()->setState(RosterEntrySynchronized);
	}
}

void JabberRosterService::prepareRoster(const QVector<Contact> &contacts)
{
	RosterService::prepareRoster(contacts);

	Q_ASSERT(StateNonInitialized == state());
	Q_ASSERT(XmppClient);

	setState(StateInitializing);
	markContactsForDeletion();

	XmppClient->rosterRequest();
}

void JabberRosterService::rosterRequestFinished(bool success)
{
	Q_ASSERT(StateInitializing == state());

	if (success)
		deleteMarkedContacts();

	setState(StateInitialized);

	emit rosterReady(success);

	executeAllTasks();
}

bool JabberRosterService::canPerformLocalUpdate() const
{
	if (!RosterService::canPerformLocalUpdate())
		return false;

	if (!XmppClient)
		return false;

	return true;
}

void JabberRosterService::executeTask(const RosterTask& task)
{
	Q_ASSERT(StateInitialized == state());

	Contact contact = ContactManager::instance()->byId(account(), task.id(), ActionReturnNull);
	XMPP::JT_Roster *rosterTask = createContactTask(contact);
	if (!rosterTask)
		return;

	RosterTaskType taskType = contact ? task.type() : RosterTaskDelete;

	if (contact)
		contact.rosterEntry()->setState(RosterEntrySynchronizing);

	switch (taskType)
	{
		case RosterTaskAdd:
			contact.setIgnoreNextStatusChange(true);
			rosterTask->set(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
			break;

		case RosterTaskDelete:
			rosterTask->remove(contact.id());
			break;

		case RosterTaskUpdate:
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
