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
#include "buddies/buddy-set-configuration-helper.h"
#include "contacts/contact.h"
#include "icons-manager.h"
#include "protocols/protocol-factory.h"
#include "status/status.h"

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
	Contact data;

	foreach (Buddy buddy, BuddyManager::instance()->buddies(CurrentAccount, true))
	{
		data = buddy.contact(CurrentAccount);
		oldStatus = data.currentStatus();

		if (oldStatus != status)
		{
			data.setCurrentStatus(status);
			emit buddyStatusChanged(CurrentAccount, buddy, oldStatus);
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

Chat * Protocol::findChat(BuddySet contacts, bool create)
{
	QList<Chat *> chats = ChatManager::instance()->chatsForAccount(account());
	foreach (Chat *c, chats)
		if (c->buddies() == contacts)
			return c;

	if (!create)
		return 0;

	Chat *chat = new Chat(account());
	ChatDetails *details = 0;

	if (contacts.count() == 1)
	{
		Buddy buddy = *contacts.begin();
		Contact contact = buddy.contact(account());
		if (contact.isNull())
		{
			delete chat;
			return 0;
		}

		ChatDetailsSimple *simple = new ChatDetailsSimple(chat);
		simple->setContact(contact);
		details = simple;
	}
	else
	{
		ChatDetailsConference *conference = new ChatDetailsConference(chat);
		conference->setBuddies(contacts);
		details = conference;
	}

	chat->setDetails(details);
	ChatManager::instance()->addChat(chat);
}

Chat * Protocol::loadChatFromStorage(StoragePoint *chatStorage)
{
	if (!chatStorage || !chatStorage->storage())
		return 0;

	XmlConfigFile *storage = chatStorage->storage();
	QDomElement point = chatStorage->point();

	Account account = AccountManager::instance()->byUuid(QUuid(storage->getTextNode(point, "Account")));

	QString type = storage->getTextNode(point, "Type");
	if ("Simple" == type)
	{
		Chat *chat = new Chat(chatStorage);
		ChatDetailsSimple *details = new ChatDetailsSimple(chat);
		chat->setDetails(details);
		chat->setState(StorableObject::StateUnloaded);
		details->setState(StorableObject::StateUnloaded);
		chat->load();
		details->load();
		return chat;
	}
	else if ("Conference" == type)
	{
		Chat *chat = new Chat(chatStorage);
		ChatDetailsConference *details = new ChatDetailsConference(chat);
		chat->setDetails(details);
		chat->setState(StorableObject::StateUnloaded);
		details->setState(StorableObject::StateUnloaded);
		chat->load();
		details->load();
		return chat;
	}
	else
		return 0;
}
