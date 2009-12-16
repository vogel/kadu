/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/type/chat-type-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "contacts/contact-shared.h"

#include "chat/chat-manager.h"

ChatManager * ChatManager::Instance = 0;

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

void ChatManager::itemRegisterd(Chat item)
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

void ChatManager::detailsLoaded(Chat chat)
{
	if (!chat.isNull())
		registerItem(chat);
}

void ChatManager::detailsUnloaded(Chat chat)
{
	if (!chat.isNull())
		unregisterItem(chat);
}

Chat ChatManager::findChat(ContactSet contacts, bool create)
{
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
	addItem(chat);

	return chat;
}
