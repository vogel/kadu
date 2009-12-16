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
#include "buddies/buddy-shared.h"

#include "contacts/contact.h"
#include "contacts/contact-manager.h"

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

	Contact contact = ContactManager::instance()->byId(ParentAccount, query, true);
	Buddy buddy = contact.ownerBuddy();
	if (buddy.isNull())
	{
		Buddy buddy = Buddy::create();
		buddy.setDisplay(QString("%1: %2").arg(ParentAccount.name()).arg(query));
		contact.setOwnerBuddy(buddy);
	}
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
