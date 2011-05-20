/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "contact-list-service.h"

ContactListService::ContactListService(Protocol *protocol) :
		QObject(protocol), CurrentProtocol(protocol)
{
}

ContactListService::~ContactListService()
{
}

Buddy ContactListService::registerBuddy(Buddy buddy)
{
	if (buddy.display().isEmpty())
		buddy.setDisplay(buddy.uuid().toString());

	Buddy resultBuddy = BuddyManager::instance()->byDisplay(buddy.display(), ActionCreate);
	resultBuddy.setAnonymous(false);

	copySupportedBuddyInformation(resultBuddy, buddy);

	QVector<Buddy> buddiesToRemove;
	bool addedSomething = false;
	foreach (const Contact &contact, buddy.contacts(CurrentProtocol->account()))
	{
		Contact knownContact = ContactManager::instance()->byId(CurrentProtocol->account(), contact.id(), ActionReturnNull);
		if (knownContact)
		{
			// do not import dirty removed contacts unless we are migrating from 0.9.x
			// (note that all migrated contacts, including those with anynomous buddies, are marked dirty)
			if (!(knownContact.isDirty() && knownContact.ownerBuddy().isAnonymous()) && isListInitiallySetUp())
			{
				if (knownContact.ownerBuddy() != resultBuddy)
				{
					// do not remove now as theoretically it may be used in next loop run
					buddiesToRemove.append(knownContact.ownerBuddy());
					knownContact.setOwnerBuddy(resultBuddy);
				}

				knownContact.setDirty(false);
				addedSomething = true;
			}
		}
		else
		{
			ContactManager::instance()->addItem(contact);
			contact.setOwnerBuddy(resultBuddy);
			contact.setDirty(false);
			addedSomething = true;
		}
	}

	if (!addedSomething)
		return Buddy::null;

	foreach (const Buddy &buddy, buddiesToRemove)
		if (buddy.contacts().isEmpty())
			BuddyManager::instance()->removeItem(buddy);

	// sometimes when a new Contact is added from server on login, sorting fails on that Contact,
	// and moving this call before the loop fixes it
	// TODO 0.10: find out why it happens and fix it _properly_ as it _might_ be a bug in model
	BuddyManager::instance()->addItem(resultBuddy);

	return resultBuddy;
}

void ContactListService::setBuddiesList(const BuddyList &buddies, bool removeOldAutomatically)
{
	QList<Contact> unImportedContacts = ContactManager::instance()->contacts(CurrentProtocol->account());

	foreach (const Contact &myselfContact, Core::instance()->myself().contacts(CurrentProtocol->account()))
		unImportedContacts.removeAll(myselfContact);

	// now buddies = SERVER_CONTACTS, unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS

	foreach (const Buddy &buddy, buddies)
	{
		Buddy managedContactsBuddy = registerBuddy(buddy);
		foreach (const Contact &contact, managedContactsBuddy.contacts())
			unImportedContacts.removeAll(contact);
	}

	// now unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS - (SERVER_CONTACTS - LOCAL_DIRTY_REMOVED_CONTACTS)
	// (unless we are importing from 0.9.x)

	QStringList contactsList;
	QList<Contact>::iterator i = unImportedContacts.begin();
	while (i != unImportedContacts.end())
	{
		Buddy ownerBuddy = i->ownerBuddy();
		if (i->isDirty() || ownerBuddy.isAnonymous())
		{
			if (i->isDirty() && ownerBuddy.isAnonymous() && !isListInitiallySetUp())
				i->setDirty(false);

			i = unImportedContacts.erase(i);
			continue;
		}

		contactsList.append(ownerBuddy.display() + " (" + i->id() + ')');

		++i;
	}

	// now unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS - (SERVER_CONTACTS - LOCAL_DIRTY_REMOVED_CONTACTS) -
	//                          - LOCAL_REMOVED_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS =
	//                        = NOT_REMOVED_LOCAL_CONTACTS - SERVER_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS =
	//                        = NOT_PRESENT_ON_SERVER_BUT_PRESENT_LOCALLY_CONTACTS - LOCAL_DIRTY_ADDED_CONTACTS
	// (unless we are importing from 0.9.x)

	if (!unImportedContacts.isEmpty())
	{
		if (removeOldAutomatically || MessageDialog::ask(KaduIcon("dialog-question"),
				tr("Kadu"),
				tr("The following contacts from your list were not found in file:<br/><b>%1</b>.<br/>"
				"Do you want to remove them from contact list?").arg(contactsList.join("</b>, <b>"))))
		{
			foreach (const Contact &contact, unImportedContacts)
			{
				Buddy ownerBuddy = contact.ownerBuddy();
				contact.setOwnerBuddy(Buddy::null);
				// remove even if it still has some data, e.g. mobile number
				if (ownerBuddy.contacts().isEmpty())
					BuddyManager::instance()->removeItem(ownerBuddy);
			}
		}
		else
			foreach (const Contact &contact, unImportedContacts)
				contact.setDirty(true);
	}

	ConfigurationManager::instance()->flush();
}

void ContactListService::importContactList()
{
	connect(this, SIGNAL(contactListImported(bool,BuddyList)),
			this, SLOT(contactListImportedSlot(bool,BuddyList)));
}

void ContactListService::contactListImportedSlot(bool ok, const BuddyList &buddies)
{
	disconnect(this, SIGNAL(contactListImported(bool,BuddyList)),
			this, SLOT(contactListImportedSlot(bool,BuddyList)));

	if (ok)
		setBuddiesList(buddies, true);
}
