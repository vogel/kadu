/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "contacts/contact-manager.h"
#include "debug.h"

#include "jabber-contact-details.h"
#include "jabber-id-validator.h"

#include "jabber-open-chat-with-runner.h"

JabberOpenChatWithRunner::JabberOpenChatWithRunner(Account account) :
		ParentAccount(account)
{
}

BuddyList JabberOpenChatWithRunner::matchingContacts(const QString &query)
{
	kdebugf();

	BuddyList matchedContacts;
	QString queryCopy(query);
	int pos = 0;
	if (JabberIdValidator::instance()->validate(queryCopy, pos) != QValidator::Acceptable)
		return matchedContacts;

	Contact contact = ContactManager::instance()->byId(ParentAccount, query, ActionCreate);
	matchedContacts.append(contact.ownerBuddy());

	return matchedContacts;
}
