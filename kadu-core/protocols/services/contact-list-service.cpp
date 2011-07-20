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

bool ContactListService::askForAddingContacts(const QMap<Buddy, Contact> &contactsToAdd, const QMap<Buddy, Contact> &contactsToRename)
{
	QString toAddString;
	QString toRenameString;

	if (!contactsToAdd.isEmpty())
	{
		QStringList contactsToAddStrings;
		for (QMap<Buddy, Contact>::const_iterator i = contactsToAdd.constBegin(); i != contactsToAdd.constEnd(); i++)
			contactsToAddStrings.append(i.key().display() + " (" + i.value().id() + ')');

		toAddString = tr("The following contacts present on server were not found on your local contact list:<br/>"
				"<b>%1</b>.<br/>").arg(contactsToAddStrings.join("</b>, <b>"));
	}

	if (!contactsToRename.isEmpty())
	{
		QStringList contactsToRenameStrings;
		for (QMap<Buddy, Contact>::const_iterator i = contactsToRename.constBegin(); i != contactsToRename.constEnd(); i++)
			contactsToRenameStrings.append(i.value().ownerBuddy().display() + " (" + i.value().id() + ") -> " + i.key().display());

		if (toAddString.isEmpty())
			toRenameString = tr("The following contacts present on server were found on your local contact list "
					"under different buddy display names:<br/><b>%1</b>.<br/>").arg(contactsToRenameStrings.join("</b>, <b>"));
		else
			toRenameString = tr("Moreover, the following contacts present on server were found on your local contact list "
					"under different buddy display names:<br/><b>%1</b>.<br/>").arg(contactsToRenameStrings.join("</b>, <b>"));
	}

	QString questionString = toAddString + toRenameString;
	if (questionString.isEmpty())
		return true;

	questionString += tr("Do you want to apply the above changes to your contact list?");

	return MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), questionString);
}

QVector<Contact> ContactListService::performAddsAndRenames(const QMap<Buddy, Contact> &contactsToAdd, const QMap<Buddy, Contact> &contactsToRename)
{
	QVector<Contact> resultContacts;

	for (QMap<Buddy, Contact>::const_iterator i = contactsToAdd.constBegin(); i != contactsToAdd.constEnd(); i++)
	{
		ContactManager::instance()->addItem(i.value());
		i.value().setOwnerBuddy(i.key());
		i.value().setDirty(false);
		resultContacts.append(i.value());
	}

	BuddyList buddiesToRemove;
	for (QMap<Buddy, Contact>::const_iterator i = contactsToRename.constBegin(); i != contactsToRename.constEnd(); i++)
	{
		// do not remove now as theoretically it could be used in next loop run
		buddiesToRemove.append(i.value().ownerBuddy());
		i.value().setOwnerBuddy(i.key());
		i.value().setDirty(false);
		resultContacts.append(i.value());
	}

	foreach (const Buddy &buddy, buddiesToRemove)
		if (buddy.contacts().isEmpty())
			BuddyManager::instance()->removeItem(buddy);

	return resultContacts;
}

QVector<Contact> ContactListService::registerBuddies(const BuddyList &buddies)
{
	QVector<Contact> resultContacts;
	QMap<Buddy, Contact> contactsToAdd;
	QMap<Buddy, Contact> contactsToRename;
	QMap<Buddy, Buddy> personalInfoSourceBuddies;

	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.display().isEmpty())
			buddy.setDisplay(buddy.uuid().toString());

		Buddy targetBuddy;
		for (QMap<Buddy, Buddy>::const_iterator i = personalInfoSourceBuddies.constBegin(); i != personalInfoSourceBuddies.constEnd(); i++)
			if (i.key().display() == buddy.display())
			{
				targetBuddy = i.key();
				break;
			}
		if (!targetBuddy)
			targetBuddy = BuddyManager::instance()->byDisplay(buddy.display(), ActionCreate);
		targetBuddy.setAnonymous(false);

		foreach (const Contact &contact, buddy.contacts(CurrentProtocol->account()))
		{
			Contact knownContact = ContactManager::instance()->byId(CurrentProtocol->account(), contact.id(), ActionReturnNull);
			if (knownContact)
			{
				// do not import dirty removed contacts unless we will be asking the user
				// (We will be asking only if we are migrating from 0.9.x. Remember that
				// all migrated contacts, including those with anynomous buddies, are initially marked dirty.)
				if (!(knownContact.isDirty() && knownContact.ownerBuddy().isAnonymous() && !haveToAskForAddingContacts()))
				{
					if (knownContact.ownerBuddy().isAnonymous())
						contactsToAdd.insert(targetBuddy, knownContact);
					else if (knownContact.ownerBuddy() != targetBuddy)
						contactsToRename.insert(targetBuddy, knownContact);
					else
					{
						knownContact.setDirty(false);
						resultContacts.append(knownContact);
					}

					personalInfoSourceBuddies.insert(targetBuddy, buddy);
				}
			}
			else
			{
				contactsToAdd.insert(targetBuddy, contact);
				personalInfoSourceBuddies.insert(targetBuddy, buddy);
			}
		}
	}

	if (haveToAskForAddingContacts() && !askForAddingContacts(contactsToAdd, contactsToRename))
		return resultContacts;

	resultContacts += performAddsAndRenames(contactsToAdd, contactsToRename);

	for (QMap<Buddy, Buddy>::const_iterator i = personalInfoSourceBuddies.constBegin(); i != personalInfoSourceBuddies.constEnd(); i++)
	{
		if (i.key().contacts().isEmpty())
			continue;

		copySupportedBuddyInformation(i.key(), i.value());
		// sometimes when a new Contact is added from server on login, sorting fails on that Contact
		// TODO 0.10: find out why it happens and fix it _properly_ as it _might_ be a bug in model
		BuddyManager::instance()->addItem(i.key());
	}

	return resultContacts;
}

void ContactListService::setBuddiesList(const BuddyList &buddies, bool removeOldAutomatically)
{
	QList<Contact> unImportedContacts = ContactManager::instance()->contacts(CurrentProtocol->account());

	foreach (const Contact &myselfContact, Core::instance()->myself().contacts(CurrentProtocol->account()))
		unImportedContacts.removeAll(myselfContact);

	// now buddies = SERVER_CONTACTS, unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS

	QVector<Contact> managedContacts = registerBuddies(buddies);
	foreach (const Contact &contact, managedContacts)
		unImportedContacts.removeAll(contact);

	// now unImportedContacts = ALL_EVER_HAD_LOCALLY_CONTACTS - (SERVER_CONTACTS - LOCAL_DIRTY_REMOVED_CONTACTS)
	// (unless we are importing from 0.9.x)

	QStringList contactsList;
	QList<Contact>::iterator i = unImportedContacts.begin();
	while (i != unImportedContacts.end())
	{
		Buddy ownerBuddy = i->ownerBuddy();
		if (i->isDirty() || ownerBuddy.isAnonymous())
		{
			// local dirty removed contacts are no longer dirty if they were absent on server
			if (i->isDirty() && ownerBuddy.isAnonymous())
				i->setDirty(false);

			i = unImportedContacts.erase(i);
		}
		else
		{
			contactsList.append(ownerBuddy.display() + " (" + i->id() + ')');
			++i;
		}
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
