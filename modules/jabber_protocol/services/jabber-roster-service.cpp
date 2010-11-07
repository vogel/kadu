/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-protocol.h"

#include "jabber-roster-service.h"

JabberRosterService::JabberRosterService(JabberProtocol *protocol) :
		QObject(protocol), Protocol(protocol), InRequest(false)
{
	connect(Protocol->client(), SIGNAL(newContact(const XMPP::RosterItem &)),
			this, SLOT(contactUpdated(const XMPP::RosterItem &)));
	connect(Protocol->client(), SIGNAL(contactUpdated(const XMPP::RosterItem &)),
			this, SLOT(contactUpdated(const XMPP::RosterItem &)));
	connect(Protocol->client(), SIGNAL(contactDeleted(const XMPP::RosterItem &)),
			this, SLOT(contactDeleted(const XMPP::RosterItem &)));
	connect(Protocol->client(), SIGNAL(rosterRequestFinished(bool)),
			this, SLOT(rosterRequestFinished(bool)));
}

JabberRosterService::~JabberRosterService()
{
}

const QString & JabberRosterService::itemDisplay(const XMPP::RosterItem &item)
{
	if (!item.name().isNull())
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
		Buddy byDispalyBuddy = BuddyManager::instance()->byDisplay(display, ActionReturnNull);
		if (byDispalyBuddy) // move to buddy by display, why not?
		{
			buddy = byDispalyBuddy;
			contact.setOwnerBuddy(byDispalyBuddy);
		}
		else
			contact.ownerBuddy().setDisplay(display);
	}
	else // check if we can change name
	{
		if (!Protocol->contactsListReadOnly())
			contact.ownerBuddy().setDisplay(display);
	}

	buddy.setAnonymous(false);

	return buddy;
}

void JabberRosterService::contactUpdated(const XMPP::RosterItem &item)
{
	kdebugf();

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

	Protocol->disconnectContactManagerSignals();

	kdebug("New roster item: %s (Subscription: %s )\n", qPrintable(item.jid().full()), qPrintable(item.subscription().toString()));

	Contact contact = ContactManager::instance()->byId(Protocol->account(), item.jid().bare(), ActionCreateAndAdd);
	ContactsForDelete.removeAll(contact);

	if (contact == Protocol->account().accountContact())
		return;

	int subType = item.subscription().type();

	// http://xmpp.org/extensions/xep-0162.html#contacts
	if (!(subType == XMPP::Subscription::Both || subType == XMPP::Subscription::To
	    || ((subType == XMPP::Subscription::None || subType == XMPP::Subscription::From) && item.ask() == "subscribe")
	    || ((subType == XMPP::Subscription::None || subType == XMPP::Subscription::From) && (!item.name().isEmpty() || !item.groups().isEmpty()))
	   ))
		return;

	Buddy buddy = itemBuddy(item, contact);
	BuddyManager::instance()->addItem(buddy);

	GroupManager *gm = GroupManager::instance();
	// add this contact to all groups the contact is a member of
	foreach (QString group, item.groups())
		buddy.addToGroup(gm->byName(group, true /* create group */));

	Protocol->connectContactManagerSignals();

	kdebugf2();
}

void JabberRosterService::contactDeleted(const XMPP::RosterItem &item)
{
	kdebug("Deleting contact %s", qPrintable(item.jid().bare()));

	Contact contact = ContactManager::instance()->byId(Protocol->account(), item.jid().bare(), ActionReturnNull);
	BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
}

void JabberRosterService::rosterRequestFinished(bool success)
{
	kdebugf();

	// the roster was imported successfully, clear
	// all "dirty" items from the contact list
	if (success)
		foreach (Contact contact, ContactsForDelete)
			BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);

	InRequest = false;
	emit rosterDownloaded(success);

	kdebugf2();
}

void JabberRosterService::downloadRoster()
{
	if (InRequest)
		return;

	InRequest = true;

	// flag roster for delete
	ContactsForDelete = ContactManager::instance()->contacts(Protocol->account());
	ContactsForDelete.removeAll(Protocol->account().accountContact());

	Protocol->client()->requestRoster();
}
