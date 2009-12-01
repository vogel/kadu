/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "chat/chat-shared.h"
#include "contacts/contact-set.h"

#include "chat.h"

Chat Chat::null;

Chat Chat::create()
{
	return new ChatShared();
}

Chat Chat::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return ChatShared::loadFromStorage(accountStoragePoint);
}

Chat::Chat()
{
}

Chat::Chat(ChatShared *data) :
		SharedBase<ChatShared>(data)
{
	data->ref.ref();
}

Chat::Chat(QObject *data)
{
	ChatShared *shared = dynamic_cast<ChatShared *>(data);
	if (shared)
		setData(shared);
}

Chat::Chat(const Chat &copy) :
		SharedBase<ChatShared>(copy)
{
}

Chat::~Chat()
{
}

Chat & Chat::operator=(const Chat &copy)
{
	clone(copy);
	return *this;
}

void Chat::refreshTitle()
{
	if (!isNull())
		data()->refreshTitle();
}


KaduSharedBase_PropertyReadDef(Chat, ContactSet, contacts, Contacts, ContactSet())
KaduSharedBase_PropertyReadDef(Chat, QString, name, Name, QString::null)
KaduSharedBase_PropertyDef(Chat, ChatDetails *, details, Details, 0)
KaduSharedBase_PropertyDef(Chat, Account, chatAccount, ChatAccount, Account::null)
KaduSharedBase_PropertyDef(Chat, QString, type, Type, QString::null)
KaduSharedBase_PropertyDef(Chat, QString, title, Title, QString::null)
KaduSharedBase_PropertyDef(Chat, QPixmap, icon, Icon, QPixmap())
