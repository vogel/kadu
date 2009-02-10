/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "contacts/contact-account-data.h"

#include "online-contact-filter.h"

OnlineContactFilter::OnlineContactFilter(QObject *parent)
	: AbstractContactFilter(parent), Enabled(false)
{
}

void OnlineContactFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool OnlineContactFilter::acceptContact(Contact contact)
{
	if (!Enabled)
		return true;

	Account *prefferedAccount = contact.prefferedAccount();
	if (!prefferedAccount)
		return false;

	return contact.accountData(prefferedAccount)->status().isOnline() || contact.accountData(prefferedAccount)->status().isBusy();
}
