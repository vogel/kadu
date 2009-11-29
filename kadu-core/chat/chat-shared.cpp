/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "chat/type/chat-type.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "chat-shared.h"

ChatShared * ChatShared::loadFromStorage(StoragePoint *storagePoint)
{
	ChatShared *result = new ChatShared();
	result->setStorage(storagePoint);
	result->ensureLoaded();

	return result;
}

ChatShared::ChatShared(QUuid uuid) :
		Shared(uuid, "Chat", ChatManager::instance()),
		ChatAccount(Account::null)
{
}

ChatShared::~ChatShared()
{
	triggerAllChatTypesUnregistered();
}

void ChatShared::load()
{
	if (!isValidStorage())
		return;

	if (!needsLoad())
		return;

	Shared::load();

	Type = loadValue<QString>("Type");
	ChatAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));

	triggerAllChatTypesRegistered();

	if (ChatAccount)
		connect(ChatAccount, SIGNAL(buddyStatusChanged(Contact, Status)),
				this, SLOT(refreshTitle()));

	refreshTitle();
}

void ChatShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Type", Type);
	storeValue("Account", ChatAccount.uuid().toString());
}

void ChatShared::emitUpdated()
{
	emit updated();
}

void ChatShared::chatTypeRegistered(ChatType *chatType)
{
	if (details())
		return;

	if (chatType->name() != Type)
		return;

	setDetails(chatType->createChatDetails(this));

	emit chatTypeLoaded();
}

void ChatShared::chatTypeUnregistered(ChatType *chatType)
{
	if (!details())
		return;

	if (chatType->name() != Type)
		return;

	setDetails(0);

	emit chatTypeUnloaded();
}

void ChatShared::detailsAdded()
{
	details()->ensureLoaded();
}

void ChatShared::detailsAboutToBeRemoved()
{
	details()->store();
}

void ChatShared::refreshTitle()
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
			foreach (const Buddy &buddy, buddies())
			{
				title.append(Parser::parse("%a", ChatAccount, buddy, false));

				if (++i < contactsSize)
					title.append(", ");
			}
		else
			foreach (const Buddy &buddy, buddies())
			{
				title.append(Parser::parse(config_file.readEntry("Look", "ConferenceContents"), ChatAccount, buddy, false));

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
				title = Parser::parse(tr("Chat with ")+"%a", ChatAccount, buddy, false);
			else
				title = Parser::parse(tr("Chat with ")+"%a (%s[: %d])", ChatAccount, buddy, false);
		}
		else
			title = Parser::parse(config_file.readEntry("Look","ChatContents"), ChatAccount, buddy, false);

		QList<Contact> contactslist = buddy.contacts(ChatAccount);
		Contact contact = contactslist.isEmpty() ? Contact::null : contactslist[0];
		if (!contact.isNull())
			Icon = ChatAccount.statusContainer()->statusPixmap(contact.currentStatus());
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

BuddySet ChatShared::buddies() const
{
	return details() ? details()->buddies() : BuddySet();
}

QString ChatShared::name() const
{
	return details() ? details()->name() : QString::null;
}
