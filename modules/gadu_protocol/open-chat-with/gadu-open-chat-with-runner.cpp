/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QIntValidator>

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"

#include "debug.h"

#include "gadu-contact-details.h"
#include "gadu-open-chat-with-runner.h"

GaduOpenChatWithRunner::GaduOpenChatWithRunner(Account account) :
		ParentAccount(account)
{
}

BuddyList GaduOpenChatWithRunner::matchingContacts(const QString &query)
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
	contact.setDetails(new GaduContactDetails(contact.storage(), contact));

	buddy.addContact(contact);
	buddy.setDisplay(ParentAccount.name() + ": " + query);
	
	matchedContacts.append(buddy);
	return matchedContacts;
}

bool GaduOpenChatWithRunner::validateUserID(const QString &uid)
{
	QIntValidator v(1, 999999999, 0);
	int pos = 0;
	QString text = uid;
	return v.validate(text, pos) == QValidator::Acceptable;
}
