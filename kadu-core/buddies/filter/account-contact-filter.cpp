/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/account-data/contact-account-data.h"

#include "account-contact-filter.h"

AccountContactFilter::AccountContactFilter(Account account, QObject *parent)
	: AbstractContactFilter(parent), Enabled(false), CurrentAccount(account)
{
}

void AccountContactFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool AccountContactFilter::acceptContact(Contact contact)
{
	if (!Enabled)
		return true;
	return contact.hasAccountData(CurrentAccount);
}

