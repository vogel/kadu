/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-list.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(Contact contact)
{
	insert(contact);
}

ContactList ContactSet::toContactList() const
{
	return toList();
}

Account * ContactSet::prefferedAccount()
{
	QList<Account *> accounts;
	// TODO 0.6.6 - Rework it if more than 1 account on the same proto.

	ContactList contacts = toList();
	foreach(Contact contact, contacts)
	{
		Account *acc = contact.prefferedAccount();
		// one contact have no account = no common account
		if (!acc)
			return 0;

		accounts.append(acc);
	}

	Account * accountMaxCount;
	int maxCount, tmp;

	foreach(Account *account, AccountManager::instance()->accounts())
	{
		tmp = accounts.count(account);
		if (maxCount < tmp)
		{
			maxCount = tmp;
			accountMaxCount = account;
		}
	}
	// TODO 0.6.6 - is it ok?
	return maxCount == contacts.count() ? accountMaxCount : 0 ;
}
