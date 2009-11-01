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
#include "chat/chat-manager.h"
#include "chat/conference-chat.h"
#include "chat/simple-chat.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set-configuration-helper.h"
#include "buddies/account-data/contact-account-data.h"
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
	ContactAccountData *data;

	foreach (Contact contact, ContactManager::instance()->contacts(CurrentAccount, true))
	{
		data = contact.accountData(CurrentAccount);
		oldStatus = data->status();

		if (oldStatus != status)
		{
			data->setStatus(status);
			emit contactStatusChanged(CurrentAccount, contact, oldStatus);
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


Chat * Protocol::findChat(ContactSet contacts, bool create)
{
	QList<Chat *> chats = ChatManager::instance()->chatsForAccount(account());
	foreach (Chat *c, chats)
		if (c->contacts() == contacts)
			return c;

	if (!create)
		return 0;

	if (contacts.count() == 1)
	{
		Contact contact = *contacts.begin();
		ContactAccountData *cad = contact.accountData(account());
		if (!cad)
			return 0;

		SimpleChat *simple = new SimpleChat(account(), cad);
		ChatManager::instance()->addChat(simple);
		return simple;
	}
	else
	{
		ConferenceChat *conference = new ConferenceChat(account(), contacts);
		ChatManager::instance()->addChat(conference);
		return conference;
	}
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
		SimpleChat *result = new SimpleChat(chatStorage);
		result->load();
		return result;
	}
	else if ("Conference" == type)
	{
		ConferenceChat *result = new ConferenceChat(chatStorage);
		result->load();
		return result;
	}
	else 
		return 0;
}
