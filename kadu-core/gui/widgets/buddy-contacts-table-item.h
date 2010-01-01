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

class BuddyContactsTableItem : public QObject
{
	Q_OBJECT

public:
	enum ItemAction {
		ItemEdit,
		ItemAdd,
		ItemDetach,
		ItemRemove
	};

private:
	Contact ItemContact;
	int ItemContactPriority;
	Account ItemAccount;
	QString Id;
	ItemAction Action;
	QString DetachedBuddyName;

	bool isAddValid() const;
	bool isEditValid() const;

public:
	explicit BuddyContactsTableItem(Contact contact = Contact::null, QObject *parent = 0);
	virtual ~BuddyContactsTableItem() {}

	Contact itemContact() const { return ItemContact; }
	void setItemContact(Contact contact);

	int itemContactPriority() const { return ItemContactPriority; }
	void setItemContactPriority(int itemContactPriority);

	Account itemAccount() const { return ItemAccount; }
	void setItemAccount(Account account);

	QString id() const { return Id; }
	void setId(const QString &id);

	ItemAction action() const { return Action; }
	void setAction(ItemAction action);

	QString detachedBuddyName() const { return DetachedBuddyName; }
	void setDetachedBuddyName(const QString &detachedBuddyName);

	bool isValid() const;

signals:
	void updated(BuddyContactsTableItem *item);

};

Q_DECLARE_METATYPE(BuddyContactsTableItem *)

#endif // BUDDY_CONTACTS_TABLE_ITEM_H
