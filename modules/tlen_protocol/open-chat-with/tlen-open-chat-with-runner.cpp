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

#include "tlen-contact-details.h"
#include "tlen-open-chat-with-runner.h"

TlenOpenChatWithRunner::TlenOpenChatWithRunner(Account account)
		: ParentAccount(account)
{
}

BuddyList TlenOpenChatWithRunner::matchingContacts(const QString &query)
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
	contact.setDetails(new TlenContactDetails(contact));

	buddy.addContact(contact);
	buddy.setDisplay(ParentAccount.name() + ": " + query);
	matchedContacts.append(buddy);

	return matchedContacts;
}


bool TlenOpenChatWithRunner::validateUserID(const QString &uid)
{
	// TODO validate ID
	// login : 3-25 ascii chars (1st letter), small letters, numbers and "-", "." , "_"
	// pass : 5-15 ascii chars, no pl chars
	//QString text = uid;

	//if (3 > text.count() || 25 < text.count())
	//	return false;

	//if (text != text.toLower())
	//	return false;

	if (!uid.contains("@"))
		return false;

	return true;
}
