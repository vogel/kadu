/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QIcon>
#include <QtGui/QTextDocument>

#include "accounts/account-manager.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-details-simple.h"
#include "chat/chat-manager.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "icons-manager.h"
#include "protocols/protocol-factory.h"
#include "status/status.h"
#include "debug.h"

#include "protocol.h"

Protocol::Protocol(Account account, ProtocolFactory *factory) :
		State(NetworkDisconnected), Factory(factory), CurrentAccount(account)
{
}

Protocol::~Protocol()
{
}

QIcon Protocol::icon()
{
	QString iconName = Factory->iconName();
	return iconName.isEmpty()
		? QIcon()
		: IconsManager::instance()->loadIcon(iconName);
}

void Protocol::setAllOffline()
{
	Status status;
	Status oldStatus;

	foreach (const Contact &contact, ContactManager::instance()->contacts(CurrentAccount))
	{
		oldStatus = contact.currentStatus();

		if (oldStatus != status)
		{
			contact.setCurrentStatus(status);
			emit buddyStatusChanged(contact, oldStatus);
		}
	}
}

void Protocol::setStatus(Status status)
{
	NextStatus = status;
	changeStatus();
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
}

void Protocol::setPrivateMode(bool privateMode)
{
	if (PrivateMode != privateMode)
	{
		PrivateMode = privateMode;
		changePrivateMode();
	}
}

void Protocol::networkStateChanged(NetworkState state)
{
	if (State == state)
		return;

	State = state;
	switch (State)
	{
		case NetworkConnecting:
			emit connecting(CurrentAccount);
			break;
		case NetworkConnected:
			emit connected(CurrentAccount);
			break;
		case NetworkDisconnecting:
			emit disconnecting(CurrentAccount);
			break;
		case NetworkDisconnected:
			emit disconnected(CurrentAccount);
			break;
	}
}
// TODO to remove
Chat Protocol::findChat(BuddySet contacts, bool create)
{
	return findChat(contacts.toContactSet(account()), create);
}

Chat Protocol::findChat(ContactSet contacts, bool create)
{
	foreach (const Chat &c, ChatManager::instance()->items())
		if (c.chatAccount() == account() && c.contacts().toBuddySet() == contacts.toBuddySet())
			return c;

	if (!create)
		return Chat::null;

	Chat chat = Chat::create();
	chat.setChatAccount(account());
	ChatDetails *details = 0;

	if (contacts.count() == 1)
	{
		Contact contact = contacts.toContact();
		if (contact.isNull())
			return Chat::null;

		ChatDetailsSimple *simple = new ChatDetailsSimple(chat);
		simple->setContact(contact);
		details = simple;
	}
	else
	{
		ChatDetailsConference *conference = new ChatDetailsConference(chat);
		conference->setContacts(contacts);
		details = conference;
	}

	chat.setDetails(details);
	ChatManager::instance()->addItem(chat);

	return chat;
}
