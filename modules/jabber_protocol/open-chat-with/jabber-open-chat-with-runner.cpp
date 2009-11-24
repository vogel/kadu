/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "buddies/buddy.h"

#include "debug.h"

#include "jabber-contact-details.h"
#include "jabber-open-chat-with-runner.h"

JabberOpenChatWithRunner::JabberOpenChatWithRunner(Account account) :
		ParentAccount(account)
{
}

BuddyList JabberOpenChatWithRunner::matchingContacts(const QString &query)
{
	kdebugf();

	BuddyList matchedContacts;
	if (!validateUserID(query))
		return matchedContacts;

	Buddy buddy;

	Contact contact;
	contact.setContactAccount(ParentAccount);
	contact.setOwnerBuddy(buddy);
	contact.setId(query);
	contact.setDetails(new JabberContactDetails(contact));

	buddy.addContact(contact);
	buddy.setDisplay(ParentAccount.name() + ": " + query);
	matchedContacts.append(buddy);

	return matchedContacts;
}

bool JabberOpenChatWithRunner::validateUserID(const QString &uid)
{
	// TODO validate ID
	//QString text = uid;
	return true;
}
