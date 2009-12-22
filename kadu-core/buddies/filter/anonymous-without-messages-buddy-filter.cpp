/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact.h"

#include "anonymous-without-messages-buddy-filter.h"

AnonymousWithoutMessagesBuddyFilter::AnonymousWithoutMessagesBuddyFilter(QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false)
{
}

void AnonymousWithoutMessagesBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool AnonymousWithoutMessagesBuddyFilter::acceptBuddy(Buddy buddy)
{
	if (!Enabled)
		return true;

// 	Account *prefferedAccount = contact.prefferedAccount();
// 	if (!prefferedAccount)
// 		return false;

// TODO:

// 	Contact contact = elem.toContact(AccountManager::instance()->defaultAccount());
// 	if (withoutMessages(contact))
// 		addUser(elem, massively, last);
// 	else
// 		removeUser(elem, massively, last);
// 
// 	removeUser(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount()));
// 
// 	UserListElement elem = UserListElement::fromContact(contact, AccountManager::instance()->defaultAccount());
// 	if (elem.isAnonymous() && withoutMessages(contact))
// 		addUser(elem);

	return !buddy.isAnonymous() || PendingMessagesManager::instance()->hasPendingMessagesForBuddy(buddy);
}
