/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "contacts/contact-shared.h"

#include "chat/chat-manager.h"

ChatManager * ChatManager::Instance = 0;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns singleton instance of ChatManager.
 * @return singleton instance of ChatManager
 */
ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
		Instance = new ChatManager();

	return Instance;
}

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
}

void ChatManager::itemAboutToBeRegistered(Chat item)
{
	emit chatAboutToBeAdded(item);
}

void ChatManager::itemRegistered(Chat item)
{
	emit chatAdded(item);
}

void ChatManager::itemAboutToBeUnregisterd(Chat item)
{
	emit chatAboutToBeRemoved(item);
}

void ChatManager::itemUnregistered(Chat item)
{
	emit chatRemoved(item);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Method is called after details for chat were loaded.
 *
 * Method is calles after details for chat were loaded. It means that
 * chat has all data loaded. It can now be registered in ChatManager
 * and itemAboutToBeAdded and itemAdded methods will be called.
 */
void ChatManager::detailsLoaded(Chat chat)
{
	QMutexLocker(&mutex());

	if (!chat.isNull())
		registerItem(chat);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Method is called after details for chat were unloaded.
 *
 * Method is calles after details for chat were unloaded. It means that
 * chat has soem data unloaded. It can now be unregistered in ChatManager
 * and itemAboutToBeRemoved and itemRemoved methods will be called.
 */
void ChatManager::detailsUnloaded(Chat chat)
{
	QMutexLocker(&mutex());

	if (!chat.isNull())
		unregisterItem(chat);
}

bool ChatManager::isAccountCommon(Account account, BuddySet buddies)
{
	QMutexLocker(&mutex());

	foreach (Buddy buddy, buddies)
		if (buddy.contacts(account).isEmpty())
			return false;

	return true;
}

Account ChatManager::getCommonAccount(BuddySet buddies)
{
	QMutexLocker(&mutex());

	QList<Account> accounts = AccountManager::instance()->items();
	foreach (Account account, accounts)
		if (isAccountCommon(account, buddies))
			return account;

	return Account::null;
}

Chat ChatManager::findChat(BuddySet buddies, bool create)
{
	QMutexLocker(&mutex());

	if (buddies.count() == 1)
	{
		Contact contact = BuddyPreferredManager::instance()->preferredContactByPendingMessages(*buddies.begin());
		if (!contact)
			contact = BuddyPreferredManager::instance()->preferredContact(*buddies.begin());

		return findChat(ContactSet(contact), create);
	}

	Account commonAccount = getCommonAccount(buddies);
	if (!commonAccount)
		return Chat::null;

	ContactSet contacts;
	foreach (Buddy buddy, buddies)
		// it is common account, so each buddy has at least one contact in this account
		contacts.insert(buddy.contacts(commonAccount).at(0));

	return findChat(contacts, create);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Finds chat for given lsit of contacts.
 * @param contacts set of contacts that builds chat
 * @param create when true new chat will be returned when none was found with contacts
 * @return chat for given lsit of contacts
 *
 * Every contact in contacts parameters needs to be in the same account. When there
 * is no contact in set or one of them is assigned to different account, Chat::null
 * will be returned.
 *
 * This method search list of all registered chats to find one with type "Simple"
 * or "Conference" that has exactly the same set of contacts like in the parameter.
 * When one is found - it is returned. Else, if create parameter is false, Chat::null
 * is returned. When it is true - new chat with type "Simple" or "Conference" (when
 * constacts set contains more than one contact) is created, added to manager,
 * fully loaded and returned.
 *
 * Do not manually create chats of type "Simple" and "Conference" - use this
 * method instead.
 */
Chat ChatManager::findChat(ContactSet contacts, bool create)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	if (contacts.size() == 0)
		return Chat::null;

	// check if every contact has the same account
	// if not true, we cannot create chat for them
	Account account = (*contacts.begin()).contactAccount();
	if (account.isNull())
		return Chat::null;

	foreach (Contact contact, contacts)
		if (account != contact.contactAccount())
			return Chat::null;

	ChatTypeManager::instance(); // load standard chat types

	foreach (const Chat &c, allItems()) // search allItems, chats can be not loaded yet
		if ((c.type() == QLatin1String("Simple") || c.type() == QLatin1String("Conference")) && c.contacts() == contacts)
			return c;

	if (!create)
		return Chat::null;

	Chat chat = Chat::create();
	chat.setChatAccount(account);
	ChatDetails *details = 0;

	Contact contact = contacts.toContact();
	if (!contact.isNull())
	{
		ChatDetailsSimple *simple = new ChatDetailsSimple(chat);
		simple->setState(StateNew);
		simple->setContact(contact);
		details = simple;
	}
	else if (contacts.size() > 1)
	{
		ChatDetailsConference *conference = new ChatDetailsConference(chat);
		conference->setState(StateNew);
		conference->setContacts(contacts);
		details = conference;
	}
	else
		return Chat::null;

	chat.setDetails(details);
	//TODO 0.6.6:
	chat.setType(details->type()->name());

	addItem(chat);
	return chat;
}
