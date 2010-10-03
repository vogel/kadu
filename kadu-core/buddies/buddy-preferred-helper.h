/*
 * %kadu copyright begin%
 * Copyright 2010, Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef BUDDY_PREFERRED_HELPER_H
#define BUDDY_PREFERRED_HELPER_H

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"

class Account;
class Buddy;
class Contact;

class KADUAPI BuddyPreferredHelper
{
	static Contact preferredContactByPendingMessages(Buddy buddy, Account account);
	static Contact preferredContactByChatWidgets(Buddy buddy, Account account);
	static Contact preferredContactByRecentChats(Buddy buddy, Account account);
	static Contact preferredContactByStatus(Buddy buddy, Account account);
	static Contact morePreferredContactByStatus(Contact c1, Contact c2, Account account = Account::null);

public:
	static Contact preferredContact(Buddy buddy, Account account, bool includechats = true);
	static Contact preferredContact(Buddy buddy, bool includechats = true);
	static Account preferredAccount(Buddy buddy, bool includechats = true);

};

#endif // BUDDY_PREFERRED_HELPER_H
