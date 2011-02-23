/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"

#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"

#include "contacts/contact-manager.h"

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

	Contact contact = ContactManager::instance()->byId(ParentAccount, query, ActionCreate);
	Buddy buddy = contact.ownerBuddy();
	if (buddy.isNull())
	{
		Buddy buddy = Buddy::create();
		buddy.setDisplay(QString("%1: %2").arg(ParentAccount.accountIdentity().name()).arg(query));
		contact.setOwnerBuddy(buddy);
	}
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
