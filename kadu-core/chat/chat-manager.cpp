/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "chat/type/chat-type-manager.h"
#include "message/message-manager.h"

#include "chat-manager.h"

ChatManager * ChatManager::Instance = 0;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns singleton instance of ChatManager.
 * @return singleton instance of ChatManager
 */
ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatManager();
		Instance->init();
	}

	return Instance;
}

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	           this, SLOT(unreadMessageAdded(Message)));
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	           this, SLOT(unreadMessageRemoved(Message)));

	foreach (const Message &message, MessageManager::instance()->allUnreadMessages())
		unreadMessageRemoved(message);
}

void ChatManager::init()
{
	foreach (const Message &message, MessageManager::instance()->allUnreadMessages())
		unreadMessageAdded(message);

	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ChatManager::itemAboutToBeRegistered(Chat item)
{
	connect(item, SIGNAL(updated()), this, SLOT(chatDataUpdated()));

	emit chatAboutToBeAdded(item);
}

void ChatManager::itemRegistered(Chat item)
{
	emit chatAdded(item);
}

void ChatManager::itemAboutToBeUnregisterd(Chat item)
{
	disconnect(item, SIGNAL(updated()), this, SLOT(chatDataUpdated()));

	emit chatAboutToBeRemoved(item);
}

void ChatManager::itemUnregistered(Chat item)
{
	emit chatRemoved(item);
}

bool ChatManager::isAccountCommon(const Account &account, const BuddySet &buddies)
{
	QMutexLocker locker(&mutex());

	foreach (const Buddy &buddy, buddies)
		if (buddy.contacts(account).isEmpty())
			return false;

	return true;
}

Account ChatManager::getCommonAccount(const BuddySet &buddies)
{
	QMutexLocker locker(&mutex());

	foreach (const Account &account, AccountManager::instance()->items())
		if (isAccountCommon(account, buddies))
			return account;

	return Account::null;
}

Chat ChatManager::findChat(const BuddySet &buddies, bool create)
{
	QMutexLocker locker(&mutex());

	if (buddies.count() == 1)
	{
		Contact contact = BuddyPreferredManager::instance()->preferredContactByUnreadMessages(*buddies.constBegin());
		if (!contact)
			contact = BuddyPreferredManager::instance()->preferredContact(*buddies.constBegin());

		return findChat(ContactSet(contact), create);
	}

	Account commonAccount = getCommonAccount(buddies);
	if (!commonAccount)
		return Chat::null;

	ContactSet contacts;
	foreach (const Buddy &buddy, buddies)
		// it is common account, so each buddy has at least one contact in this account
		contacts.insert(buddy.contacts(commonAccount).at(0));

	if (contacts.size() != buddies.size())
		return Chat::null;

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
Chat ChatManager::findChat(const ContactSet &contacts, bool create)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (contacts.isEmpty())
		return Chat::null;

	// check if every contact has the same account
	// if not true, we cannot create chat for them
	Account account = (*contacts.constBegin()).contactAccount();
	if (account.isNull())
		return Chat::null;

	foreach (const Contact &contact, contacts)
		if (account != contact.contactAccount())
			return Chat::null;

	ChatTypeManager::instance(); // load standard chat types

	// TODO #1694
	// for some users that have self on user list
	// this should not be possible, and prevented on other level (like in ContactManager)
	foreach (const Contact &contact, contacts)
	{
		if (contact.id() == account.id())
			return Chat::null;
	}

	foreach (const Chat &c, allItems()) // search allItems, chats can be not loaded yet
		if ((c.type() == QLatin1String("Simple") || c.type() == QLatin1String("Conference")) && c.contacts() == contacts)
		{
			// when contacts changed their accounts we need to change account of chat too
			c.setChatAccount(account);
			return c;
		}

	if (!create)
		return Chat::null;

	Chat chat = Chat::create();
	chat.setChatAccount(account);

	Contact contact = contacts.toContact();
	if (!contact.isNull())
	{
		chat.setType("Simple");

		ChatDetailsSimple *simple = dynamic_cast<ChatDetailsSimple *>(chat.details());
		simple->setState(StateNew);
		simple->setContact(contact);
	}
	else if (contacts.size() > 1)
	{
		// only gadu-gadu support conferences
		// TODO: this should be done better
		if (chat.chatAccount().protocolName() != "gadu")
			return Chat::null;

		chat.setType("Conference");

		ChatDetailsConference *conference = dynamic_cast<ChatDetailsConference *>(chat.details());
		conference->setState(StateNew);
		conference->setContacts(contacts);
	}
	else
		return Chat::null;

	addItem(chat);
	return chat;
}

Chat ChatManager::byDisplay(const QString &display)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (display.isEmpty())
		return Chat::null;

	foreach (const Chat &chat, allItems())
		if (display == chat.display())
			return chat;

	return Chat::null;
}

void ChatManager::chatDataUpdated()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatUpdated(chat);
}

void ChatManager::unreadMessageAdded(const Message &message)
{
	const Chat &chat = message.messageChat();
	chat.setUnreadMessagesCount(chat.unreadMessagesCount() + 1);
}

void ChatManager::unreadMessageRemoved(const Message &message)
{
	const Chat &chat = message.messageChat();
	quint16 unreadMessagesCount = chat.unreadMessagesCount();
	if (unreadMessagesCount > 0)
		chat.setUnreadMessagesCount(unreadMessagesCount - 1);
}
