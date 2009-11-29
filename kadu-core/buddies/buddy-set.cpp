/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "buddies/buddy-list.h"

#include "buddy-set.h"

BuddySet::BuddySet()
{
}

BuddySet::BuddySet(Buddy buddy)
{
	insert(buddy);
}

BuddyList BuddySet::toBuddyList() const
{
	return toList();
}

QList<Contact> BuddySet::toContactList(Account account) const
{
	Account acc(account.isNull() ? this->prefferedAccount() : account);

	// if not have same account return empty list
	if (acc.isNull())
		return QList<Contact>();
	
	QList<Contact> contacts;
	foreach (const Buddy &buddy, toList())
	{
		// TODO 0.6.6: change to buddy.contact(acc) ??
		Contact tmp = ContactManager::instance()->byId(acc, buddy.id(acc));
		if (tmp != Contact::null)
			contacts.append(tmp);
	}

	return contacts;
}

ContactSet BuddySet::toContactSet(Account account) const
{
	Account acc(account.isNull() ? this->prefferedAccount() : account);

	// if not have same account return empty list
	if (acc.isNull())
		return ContactSet();
	
	ContactSet contacts;
	foreach (const Buddy &buddy, toList())
	{
		// TODO 0.6.6: change to buddy.contact(acc) ??
		Contact tmp = ContactManager::instance()->byId(acc, buddy.id(acc));
		if (tmp != Contact::null)
			contacts.insert(tmp);
	}

	return contacts;

}

QList<Contact> BuddySet::toAllContactList() const
{
	QList<Contact> contacts;
	foreach (const Buddy &buddy, toList())
		contacts.append(buddy.contacts());

	return contacts;
}

Account BuddySet::prefferedAccount() const
{
	QList<Account> accounts;
	QList<Account> contactAccounts;
	int contactsCount = count();
	// TODO 0.6.6 - Rework it if more than 1 account on the same proto.

	foreach (const Buddy &buddy, toList())
	{
		contactAccounts = buddy.accounts();
		// one contact have no account = no common account
		if (0 == contactAccounts.count())
			return Account::null;

		accounts.append(contactAccounts);
	}

	foreach (const Account &account, AccountManager::instance()->accounts())
		if (contactsCount == accounts.count(account))
			return account;

	return Account::null;
}
