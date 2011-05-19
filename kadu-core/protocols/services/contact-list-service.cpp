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
	
	// TODO: generate this somehow based on which information the actual contact list service can provide
	resultBuddy.setFirstName(buddy.firstName());
	resultBuddy.setLastName(buddy.lastName());
	resultBuddy.setNickName(buddy.nickName());
	resultBuddy.setMobile(buddy.mobile());
	resultBuddy.setGroups(buddy.groups());
	resultBuddy.setEmail(buddy.email());
	resultBuddy.setDisplay(buddy.display());
	resultBuddy.setHomePhone(buddy.homePhone());
	resultBuddy.setOfflineTo(buddy.isOfflineTo());
	resultBuddy.setCity(buddy.city());
	resultBuddy.setWebsite(buddy.website());
	resultBuddy.setGender(buddy.gender());

	bool addedSomething = false;
	foreach (const Contact &contact, buddy.contacts(CurrentProtocol->account()))
	{
		Contact knownContact = ContactManager::instance()->byId(CurrentProtocol->account(), contact.id(), ActionReturnNull);
		if (knownContact)
		{
			if (knownContact.ownerBuddy().isAnonymous() && RosterStatusDirtyRemoved == knownContact.rosterStatus())
				BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(knownContact);
			else if (knownContact.ownerBuddy() != resultBuddy)
			{
				BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(knownContact);
				knownContact.setOwnerBuddy(resultBuddy);
				addedSomething = true;
			}
			else
				addedSomething = true;

			knownContact.setRosterStatus(RosterStatusNormal);
		}
		else
		{
			ContactManager::instance()->addItem(contact);
			contact.setRosterStatus(RosterStatusNormal);
			contact.setOwnerBuddy(resultBuddy);
			addedSomething = true;
		}
	}
	
	if (!addedSomething)
		return Buddy::null;

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

	foreach (const Buddy &buddy, buddies)
	{
		Buddy managedContactsBuddy = registerBuddy(buddy);
		foreach (const Contact &contact, managedContactsBuddy.contacts())
			unImportedContacts.removeAll(contact);
	}

	QStringList contactsList;
	QList<Contact>::iterator i = unImportedContacts.begin();
	while (i != unImportedContacts.end())
	{
		Buddy ownerBuddy = i->ownerBuddy();
		if (RosterStatusDirtyAdded == i->rosterStatus() || ownerBuddy.isAnonymous())
		{
			i = unImportedContacts.erase(i);
			continue;
		}

		contactsList.append(ownerBuddy.display() + " (" + i->id() + ')');

		++i;
	}

	if (!unImportedContacts.isEmpty())
	{
		if (removeOldAutomatically || MessageDialog::ask(KaduIcon("dialog-question"),
				tr("Kadu - Account %1 (%2)"),
				tr("The following contacts from your list for account %1 (%2) were not found on server:<br/><b>%3</b>.<br/>"
				"Do you want to remove them from contact list?")
				.arg(CurrentProtocol->account().accountIdentity().name()).arg(CurrentProtocol->account().id())
				.arg(contactsList.join("</b>, <b>"))))
		{
			foreach (const Contact &contact, unImportedContacts)
			{
				BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
				contact.setRosterStatus(RosterStatusNormal);
			}
		}
		else
			foreach (const Contact &contact, unImportedContacts)
				contact.setRosterStatus(RosterStatusDirtyAdded);
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
		setBuddiesList(buddies, shouldDeleteOldContactsAutomatically());
}
