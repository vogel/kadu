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

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "debug.h"

#include "services/jabber-subscription-service.h"
#include "jabber-protocol.h"

#include "jabber-roster-service.h"

JabberRosterService::JabberRosterService(JabberProtocol *protocol) :
		RosterService(protocol)
{
	connect(protocol->client(), SIGNAL(newContact(const XMPP::RosterItem &)),
			this, SLOT(contactUpdated(const XMPP::RosterItem &)));
	connect(protocol->client(), SIGNAL(contactUpdated(const XMPP::RosterItem &)),
			this, SLOT(contactUpdated(const XMPP::RosterItem &)));
	connect(protocol->client(), SIGNAL(contactDeleted(const XMPP::RosterItem &)),
			this, SLOT(contactDeleted(const XMPP::RosterItem &)));
	connect(protocol->client(), SIGNAL(rosterRequestFinished(bool)),
			this, SLOT(rosterRequestFinished(bool)));
}

JabberRosterService::~JabberRosterService()
{
}

const QString & JabberRosterService::itemDisplay(const XMPP::RosterItem &item)
{
	if (!item.name().isEmpty())
		return item.name();
	else
		return item.jid().bare();
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

	JabberProtocol *jabberProtocol = static_cast<JabberProtocol *>(protocol());

	kdebug("New roster item: %s (Subscription: %s )\n", qPrintable(item.jid().full()), qPrintable(item.subscription().toString()));

	Contact contact = ContactManager::instance()->byId(protocol()->account(), item.jid().bare(), ActionCreateAndAdd);
	// in case we return before next call of it
	contact.setDirty(false);
	ContactsForDelete.removeAll(contact);

	if (contact == jabberProtocol->account().accountContact())
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

	// Facebook Chat does not support groups. So make Facebook contacts not remove their
	// owner buddies (which may own more contacts) from their groups. See bug #2320.
	if (!jabberProtocol->contactsListReadOnly())
	{
		QList<Group> groups;
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

	kdebug("Deleting contact %s\n", qPrintable(item.jid().bare()));

	Contact contact = ContactManager::instance()->byId(protocol()->account(), item.jid().bare(), ActionReturnNull);
	BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
	contact.setDirty(false);

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

	setState(StateInitializing);

	// flag roster for delete
	ContactsForDelete = ContactManager::instance()->contacts(protocol()->account()).toList();
	ContactsForDelete.removeAll(protocol()->account().accountContact());

	static_cast<JabberProtocol *>(protocol())->client()->requestRoster();
}

bool JabberRosterService::canPerformLocalUpdate() const
{
	if (!RosterService::canPerformLocalUpdate())
		return false;

	if (!static_cast<JabberProtocol *>(protocol())->client())
		return false;

	return true;
}

void JabberRosterService::addContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != protocol()->account() || contact.isAnonymous())
		return;

	Buddy buddy = contact.ownerBuddy();
	QStringList groupsList;

	foreach (const Group &group, buddy.groups())
		groupsList.append(group.name());

	static_cast<JabberProtocol *>(protocol())->client()->addContact(contact.id(), contact.display(true), groupsList);
	contact.setDirty(false);
}

void JabberRosterService::removeContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != protocol()->account())
		return;

	static_cast<JabberProtocol *>(protocol())->client()->removeContact(contact.id());
	contact.setDirty(false);
}

void JabberRosterService::updateContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() || contact.contactAccount() != protocol()->account() || contact.isAnonymous())
		return;

	QStringList groupsList;
	foreach (const Group &group, contact.ownerBuddy().groups())
		groupsList.append(group.name());

	static_cast<JabberProtocol *>(protocol())->client()->updateContact(contact.id(), contact.display(true), groupsList);
}
