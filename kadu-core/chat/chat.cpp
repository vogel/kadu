/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "chat/chat-details.h"
#include "chat/chat-details-simple.h"
#include "chat/chat-details-conference.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"
#include "protocols/protocol.h"

#include "chat.h"

Chat * Chat::loadFromStorage(StoragePoint *chatStoragePoint)
{
	if (!chatStoragePoint || !chatStoragePoint->storage())
		return 0;

	XmlConfigFile *storage = chatStoragePoint->storage();
	QDomElement point = chatStoragePoint->point();

	Account account = AccountManager::instance()->byUuid(QUuid(storage->getTextNode(point, "Account")));

	QString type = storage->getTextNode(point, "Type");
	if ("Simple" == type)
	{
		Chat *chat = new Chat(chatStoragePoint);
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
		Chat *chat = new Chat(chatStoragePoint);
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

Chat::Chat(StoragePoint *storage) :
		UuidStorableObject(storage), CurrentAccount(Account::null), Details(0)
{
}

Chat::Chat(Account currentAccount, QUuid uuid) :
		UuidStorableObject("Chat", ChatManager::instance()), CurrentAccount(currentAccount),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Details(0)
{
	connect(CurrentAccount, SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(refreshTitle()));
}

Chat::~Chat()
{
	disconnect(CurrentAccount, SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(refreshTitle()));
}

void Chat::load()
{
	if (!isValidStorage())
		return;

	if (!needsLoad())
		return;

	UuidStorableObject::load();

	Uuid = loadAttribute<QString>("uuid");
	CurrentAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));

	connect(CurrentAccount, SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(refreshTitle()));
	refreshTitle();
}

void Chat::store()
{
	if (!isValidStorage())
		return;

	storeValue("Account", CurrentAccount.uuid().toString());
}

void Chat::setTitle(const QString &newTitle)
{
	if (Title == newTitle)
		return;
	Title = newTitle;
	emit titleChanged(this, newTitle);
}

void Chat::refreshTitle()
{
	kdebugf();
	QString title;

	int contactsSize = buddies().count();
	kdebugmf(KDEBUG_FUNCTION_START, "contacts().size() = %d\n", contactsSize);
	if (contactsSize > 1)
	{
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		int i = 0;

		if (config_file.readEntry("Look", "ConferenceContents").isEmpty())
			foreach(const Buddy buddy, buddies())
			{
				title.append(Parser::parse("%a", account(), buddy, false));

				if (++i < contactsSize)
					title.append(", ");
			}
		else
			foreach(const Buddy buddy, buddies())
			{
				title.append(Parser::parse(config_file.readEntry("Look", "ConferenceContents"), account(), buddy, false));

				if (++i < contactsSize)
					title.append(", ");
			}

 		Icon = IconsManager::instance()->loadPixmap("Online");
	}
	else if (contactsSize > 0)
	{
		Buddy buddy = *buddies().begin();

		if (config_file.readEntry("Look", "ChatContents").isEmpty())
		{
			if (buddy.isAnonymous())
				title = Parser::parse(tr("Chat with ")+"%a", account(), buddy, false);
			else
				title = Parser::parse(tr("Chat with ")+"%a (%s[: %d])", account(), buddy, false);
		}
		else
			title = Parser::parse(config_file.readEntry("Look","ChatContents"), account(), buddy, false);

		Contact contact = buddy.contact(account());
		if (!contact.isNull())
			Icon = account().statusContainer()->statusPixmap(contact.currentStatus());
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

ChatType * Chat::type() const
{
	return Details ? Details->type() : 0;
}

BuddySet Chat::buddies() const
{
	return Details ? Details->buddies() : BuddySet();
}

QString Chat::name() const
{
	return Details ? Details->name() : QString::null;
}
