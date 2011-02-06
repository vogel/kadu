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

Buddy ContactListService::mergeBuddy(Buddy oneBuddy)
{
	Buddy buddy;
	QList<Contact> oneBuddyContacts = oneBuddy.contacts(CurrentProtocol->account());
	if (oneBuddy.display().isEmpty())
		oneBuddy.setDisplay(oneBuddy.uuid().toString());

	if (oneBuddyContacts.count() > 0)
	{
		foreach (const Contact &contact, oneBuddyContacts)
		{
			Contact contactOnList = ContactManager::instance()->byId(CurrentProtocol->account(), contact.id(), ActionReturnNull);
			if (!contactOnList) // not on list add this one as new
			{
				// move contact to buddy
				ContactManager::instance()->addItem(contact);
				buddy = BuddyManager::instance()->byDisplay(oneBuddy.display(), ActionCreate);

				contact.setOwnerBuddy(buddy);

				kdebugmf(KDEBUG_FUNCTION_START, "\nuuid add: '%s' %s\n",
					 qPrintable(contact.uuid().toString()), qPrintable(buddy.display()));
			}
			else // already on list
			{
				// found contact so use his buddy
				//kdebugmf(KDEBUG_FUNCTION_START, "\nuuid before: '%s'\n", qPrintable(contactOnList.ownerBuddy().uuid().toString()));
				buddy = contactOnList.ownerBuddy();
				kdebugmf(KDEBUG_FUNCTION_START, "\nuuid owner: '%s' %s\n",
					 qPrintable(contactOnList.uuid().toString()), qPrintable(buddy.display()));
				//unImportedContacts.removeOne(contactOnList);
			}
		}
	}
	else
	{
		// display is never null, we set it above, so buddy is never null
		buddy = BuddyManager::instance()->byDisplay(oneBuddy.display(), ActionCreateAndAdd);
	}

	// if this buddy really should be merged with another, this is user's job!
	buddy.setFirstName(oneBuddy.firstName());
	buddy.setLastName(oneBuddy.lastName());
	buddy.setNickName(oneBuddy.nickName());
	buddy.setMobile(oneBuddy.mobile());
	buddy.setGroups(oneBuddy.groups());
	buddy.setEmail(oneBuddy.email());
	buddy.setDisplay(oneBuddy.display());
	buddy.setHomePhone(oneBuddy.homePhone());
	buddy.setOfflineTo(oneBuddy.isOfflineTo());
	buddy.setCity(oneBuddy.city());
	buddy.setWebsite(oneBuddy.website());
	buddy.setGender(oneBuddy.gender());
	buddy.setAnonymous(false);

	BuddyManager::instance()->addItem(buddy);

	return buddy;
}

void ContactListService::setBuddiesList(const BuddyList &buddies, bool removeOld)
{
	QList<Contact> unImportedContacts = ContactManager::instance()->contacts(CurrentProtocol->account());

	foreach (const Contact &c, Core::instance()->myself().contacts(CurrentProtocol->account()))
		unImportedContacts.removeAll(c);

	foreach (const Buddy &oneBuddy, buddies)
	{
		Buddy mergedBuddy = mergeBuddy(oneBuddy);
		foreach (const Contact &contact, mergedBuddy.contacts(CurrentProtocol->account()))
			unImportedContacts.removeAll(contact);
	}

	QStringList contactsList;
	QList<Contact>::iterator i = unImportedContacts.begin();
	while (i != unImportedContacts.end())
	{
		// TODO: why not Contact.ownerBuddy()?
		Buddy buddy = BuddyManager::instance()->byId(CurrentProtocol->account(), i->id(), ActionCreate);
		if (buddy.isAnonymous())
		{
			i = unImportedContacts.erase(i);
			continue;
		}

		if (!contactsList.contains(buddy.display()))
			contactsList.append(buddy.display());

		++i;
	}

	if (removeOld && !unImportedContacts.isEmpty())
	{
		if (MessageDialog::ask(QString(), tr("Kadu"), tr("Following contacts from your list were not found on server: <b>%0</b>.\nDo you want to remove them from contacts list?").arg(contactsList.join("</b>, <b>"))))
		{
			foreach (const Contact &contact, unImportedContacts)
				BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		}
	}

	// flush configuration to save all changes
	ConfigurationManager::instance()->flush();
}

void ContactListService::importContactList(bool automaticallySetBuddiesList)
{
	if (automaticallySetBuddiesList)
		connect(this, SIGNAL(contactListImported(bool,BuddyList)),
				this, SLOT(contactListImportedSlot(bool,BuddyList)));
}

void ContactListService::contactListImportedSlot(bool ok, const BuddyList &buddies)
{
	disconnect(this, SIGNAL(contactListImported(bool,BuddyList)),
			this, SLOT(contactListImportedSlot(bool,BuddyList)));

	if (ok)
		setBuddiesList(buddies);
}
