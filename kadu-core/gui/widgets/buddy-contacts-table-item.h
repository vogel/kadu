/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACTS_TABLE_ITEM_H
#define BUDDY_CONTACTS_TABLE_ITEM_H

#include "accounts/account.h"
#include "contacts/contact.h"

class BuddyContactsTableItem
{
	Contact ItemContact;
	Account ItemAccount;
	QString Id;

public:
	BuddyContactsTableItem(Contact contact = Contact::null);
	~BuddyContactsTableItem();

	Contact itemContact() { return ItemContact; }
	void setItemContact(Contact contact) { ItemContact = contact; }

	Account itemAccount() { return ItemAccount; }
	void setItemAccount(Account account) { ItemAccount = account; }

	QString id() { return Id; }
	void setId(const QString &id) { Id = id; }

};

#endif // BUDDY_CONTACTS_TABLE_ITEM_H
