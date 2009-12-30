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
public:
	enum ItemAction {
		ItemEdit,
		ItemAdd,
		ItemDetach,
		ItemRemove
	};

private:
	Contact ItemContact;
	Account ItemAccount;
	QString Id;
	ItemAction Action;
	QString DetachedBuddyName;

public:
	BuddyContactsTableItem(Contact contact = Contact::null);
	~BuddyContactsTableItem() {}

	Contact itemContact() const { return ItemContact; }
	void setItemContact(Contact contact) { ItemContact = contact; }

	Account itemAccount() const { return ItemAccount; }
	void setItemAccount(Account account) { ItemAccount = account; }

	QString id() const { return Id; }
	void setId(const QString &id) { Id = id; }

	ItemAction action() const { return Action; }
	void setAction(ItemAction action) { Action = action; }

	QString detachedBuddyName() const { return DetachedBuddyName; }
	void setDetachedBuddyName(const QString &detachedBuddyName) { DetachedBuddyName = detachedBuddyName; }

};

#endif // BUDDY_CONTACTS_TABLE_ITEM_H
