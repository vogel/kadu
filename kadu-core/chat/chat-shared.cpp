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
#include "contacts/contact-set.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "chat-shared.h"

ChatShared * ChatShared::loadFromStorage(StoragePoint *storagePoint)
{
	ChatShared *result = new ChatShared();
	result->setStorage(storagePoint);

	return result;
}

ChatShared::ChatShared(QUuid uuid) :
		Shared(uuid), ChatAccount(Account::null)
{
}

ChatShared::~ChatShared()
{
	triggerAllChatTypesUnregistered();
}

StorableObject * ChatShared::storageParent()
{
	return ChatManager::instance();
}

QString ChatShared::storageNodeName()
{
	return QLatin1String("Chat");
}

void ChatShared::load()
{
	if (!isValidStorage())
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
	ensureLoaded();

	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Type", Type);
	storeValue("Account", ChatAccount.uuid().toString());

	if (details())
		details()->store();
}

bool ChatShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore()
			&& !ChatAccount.uuid().isNull()
			&& (!details() || details()->shouldStore());
}

void ChatShared::aboutToBeRemoved()
{
	ChatAccount = Account::null;
	setDetails(0);
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
}

void ChatShared::chatTypeUnregistered(ChatType *chatType)
{
	if (!details())
		return;

	if (chatType->name() != Type)
		return;

	setDetails(0);
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

	int contactsSize = contacts().count();
	kdebugmf(KDEBUG_FUNCTION_START, "contacts().size() = %d\n", contactsSize);
	if (contactsSize > 1)
	{
		title = config_file.readEntry("Look","ConferencePrefix");
		if (title.isEmpty())
			title = tr("Conference with ");

		QString conferenceContents = config_file.readEntry("Look", "ConferenceContents");
		QStringList contactslist;
		foreach (const Buddy &buddy, contacts().toBuddySet())
			contactslist.append(Parser::parse(conferenceContents.isEmpty() ? "%a" : conferenceContents, ChatAccount, buddy, false));
	
		title.append(contactslist.join(", "));

 		Icon = IconsManager::instance()->loadPixmap("Online");
	}
	else if (contactsSize > 0)
	{
		Contact contact = contacts().toContact();
		Buddy buddy = contact.ownerBuddy();

		if (config_file.readEntry("Look", "ChatContents").isEmpty())
		{
			if (buddy.isAnonymous())
				title = Parser::parse(tr("Chat with ")+"%a", ChatAccount, buddy, false);
			else
				title = Parser::parse(tr("Chat with ")+"%a (%s[: %d])", ChatAccount, buddy, false);
		}
		else
			title = Parser::parse(config_file.readEntry("Look","ChatContents"), ChatAccount, buddy, false);

		if (!contact.isNull() && ChatAccount.statusContainer())
			Icon = ChatAccount.statusContainer()->statusPixmap(contact.currentStatus());
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

ContactSet ChatShared::contacts()
{
	ensureLoaded();

	return details() ? details()->contacts() : ContactSet();
}

QString ChatShared::name()
{
	ensureLoaded();

	return details() ? details()->name() : QString::null;
}
