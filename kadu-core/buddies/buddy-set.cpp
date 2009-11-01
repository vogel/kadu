/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/buddy-list.h"

#include "buddy-set.h"

BuddySet::BuddySet()
{
}

BuddySet::BuddySet(Buddy contact)
{
	insert(contact);
}

BuddyList BuddySet::toBuddyList() const
{
	return toList();
}

Account BuddySet::prefferedAccount()
{
	QList<Account> accounts;
	QList<Account> contactAccounts;
	int contactsCount = count();
	// TODO 0.6.6 - Rework it if more than 1 account on the same proto.

	foreach (Buddy contact, toList())
	{
		contactAccounts = contact.accounts();
		// one contact have no account = no common account
		if (0 == contactAccounts.count())
			return Account::null;

		accounts.append(contactAccounts);
	}

	foreach (Account account, AccountManager::instance()->accounts())
		if (contactsCount == accounts.count(account))
			return account;

	return Account::null;
}
