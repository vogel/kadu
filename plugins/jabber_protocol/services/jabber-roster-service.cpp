/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

JabberRosterService::JabberRosterService(JabberProtocol *protocol) :
		RosterService(protocol), XmppClient(0)
{
	Q_ASSERT(protocol);
}

JabberRosterService::~JabberRosterService()
{
}

void JabberRosterService::connectToClient()
{
	if (!XmppClient)
		return;

	connect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));
	connect(XmppClient, SIGNAL(rosterItemAdded(const RosterItem &)),
	        this, SLOT(contactUpdated(const RosterItem &)));
	connect(XmppClient, SIGNAL(rosterItemUpdated(const RosterItem &)),
	        this, SLOT(contactUpdated(const RosterItem &)));
	connect(XmppClient, SIGNAL(rosterItemRemoved(const RosterItem &)),
	        this, SLOT(contactDeleted(const RosterItem &)));
	connect(XmppClient, SIGNAL(rosterRequestFinished(bool, int, QString)),
	        this, SLOT(rosterRequestFinished(bool)));
}

void JabberRosterService::disconnectFromClient()
{
	if (!XmppClient)
		return;

	disconnect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));
	disconnect(XmppClient, SIGNAL(rosterItemAdded(const RosterItem &)),
	           this, SLOT(contactUpdated(const RosterItem &)));
	disconnect(XmppClient, SIGNAL(rosterItemUpdated(const RosterItem &)),
	           this, SLOT(contactUpdated(const RosterItem &)));
	disconnect(XmppClient, SIGNAL(rosterItemRemoved(const RosterItem &)),
	           this, SLOT(contactDeleted(const RosterItem &)));
	disconnect(XmppClient, SIGNAL(rosterRequestFinished(bool, int, QString)),
	           this, SLOT(rosterRequestFinished(bool)));
}

void JabberRosterService::clientDestroyed()
{
	XmppClient = 0;
}

void JabberRosterService::setClient(Client *xmppClient)
{
	disconnectFromClient();
	XmppClient = xmppClient;
	connectToClient();
}

Buddy JabberRosterService::itemBuddy(const XMPP::RosterItem &item, const Contact &contact)
{
	QString display = itemDisplay(item);
	Buddy buddy = contact.ownerBuddy();
	if (buddy.isAnonymous()) // contact has anonymous buddy, we should search for other
	{
		Buddy byDisplayBuddy = BuddyManager::instance()->byDisplay(display, ActionReturnNull);
		if (byDisplayBuddy) // move to buddy by display, why not?
		{
			buddy = byDisplayBuddy;
			contact.setOwnerBuddy(buddy);
		}
		else
		{
			if (!buddy)
				buddy = Buddy::create();
			buddy.setDisplay(display);
		}

		buddy.setAnonymous(false);
	}
	else
	{
		// Prevent read-only rosters (e.g., Facebook Chat) from changing names of buddies with multiple contacts (#1570).
		// Though, if given buddy has exactly one contact, let the name be changed (#2226).
		if (!protocol()->contactsListReadOnly() || 1 == buddy.contacts().count())
			buddy.setDisplay(display);
	}

	return buddy;
}

void JabberRosterService::contactUpdated(const XMPP::RosterItem &item)
{
	kdebugf();

	// StateInitialized - this is new update from roster
	// StateInitializing - this is initial data from roster
	RosterState originalState = state();
	if (StateInitialized != originalState && StateInitializing != originalState)
		return;

	setState(StateProcessingRemoteUpdate);

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

	kdebug("New roster item: %s (Subscription: %s )\n", qPrintable(item.jid().full()), qPrintable(item.subscription().toString()));

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionCreateAndAdd);

	// in case we return before next call of it
	contact.setDirty(false);
	ContactsForDelete.removeAll(contact);

	if (contact == account().accountContact())
	{
		setState(originalState);
		return;
	}

	int subType = item.subscription().type();

	// http://xmpp.org/extensions/xep-0162.html#contacts
	if (!(subType == XMPP::Subscription::Both || subType == XMPP::Subscription::To
	    || ((subType == XMPP::Subscription::None || subType == XMPP::Subscription::From) && item.ask() == "subscribe")
	    || ((subType == XMPP::Subscription::None || subType == XMPP::Subscription::From) && (!item.name().isEmpty() || !item.groups().isEmpty()))
	   ))
	{
		setState(originalState);
		return;
	}

	Buddy buddy = itemBuddy(item, contact);
	BuddyManager::instance()->addItem(buddy);

	RosterService::addContact(contact);

	// Facebook Chat does not support groups. So make Facebook contacts not remove their
	// owner buddies (which may own more contacts) from their groups. See bug #2320.
	if (!protocol()->contactsListReadOnly())
	{
		QSet<Group> groups;
		foreach (const QString &group, item.groups())
			groups << GroupManager::instance()->byName(group);
		buddy.setGroups(groups);
	}

	contact.setDirty(false);

	setState(originalState);

	kdebugf2();
}

void JabberRosterService::contactDeleted(const XMPP::RosterItem &item)
{
	// StateInitialized - this is new update from roster
	// StateInitializing - this is initial data from roster
	RosterState originalState = state();
	if (StateInitialized != originalState && StateInitializing != originalState)
		return;

	setState(StateProcessingRemoteUpdate);

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);
	BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
	contact.setDirty(false);

	RosterService::removeContact(contact);

	setState(originalState);
}

void JabberRosterService::rosterRequestFinished(bool success)
{
	kdebugf();

	Q_ASSERT(StateInitializing == state());

	// the roster was imported successfully, clear
	// all "dirty" items from the contact list
	if (success)
		foreach (const Contact &contact, ContactsForDelete)
		{
			BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
			contact.setDirty(false);
		}

	setState(StateInitialized);

	emit rosterReady(success);

	kdebugf2();
}

void JabberRosterService::prepareRoster()
{
	Q_ASSERT(StateNonInitialized == state());
	Q_ASSERT(XmppClient);

	setState(StateInitializing);

	// flag roster for delete
	ContactsForDelete = ContactManager::instance()->contacts(account()).toList();
	ContactsForDelete.removeAll(account().accountContact());

	XmppClient->rosterRequest();
}

bool JabberRosterService::canPerformLocalUpdate() const
{
	if (!RosterService::canPerformLocalUpdate())
		return false;

	if (!XmppClient)
		return false;

	return true;
}

bool JabberRosterService::addContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != account() || contact.isAnonymous())
		return false;

	Q_ASSERT(StateInitialized == state());

	if (!RosterService::addContact(contact))
		return false;

	setState(StateProcessingLocalUpdate);

	// see issue #2159 - we need a way to ignore first status of given contact
	contact.setIgnoreNextStatusChange(true);

	XMPP::JT_Roster *rosterTask = new XMPP::JT_Roster(XmppClient->rootTask());
	rosterTask->set(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
	rosterTask->go(true);

	contact.setDirty(false);

	setState(StateInitialized);

	return true;
}

bool JabberRosterService::removeContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != account())
		return false;

	Q_ASSERT(StateInitialized == state());

	if (!RosterService::removeContact(contact))
		return false;

	setState(StateProcessingLocalUpdate);

	XMPP::JT_Roster *rosterTask = new XMPP::JT_Roster(XmppClient->rootTask());
	rosterTask->remove(contact.id());
	rosterTask->go(true);

	contact.setDirty(false);

	setState(StateInitialized);

	return true;
}

void JabberRosterService::updateContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != account() || contact.isAnonymous())
		return;

	Q_ASSERT(StateInitialized == state());

	setState(StateProcessingLocalUpdate);

	XMPP::JT_Roster *rosterTask = new XMPP::JT_Roster(XmppClient->rootTask());
	rosterTask->set(contact.id(), contact.display(true), buddyGroups(contact.ownerBuddy()));
	rosterTask->go(true);

	setState(StateInitialized);
}

}
