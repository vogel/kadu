/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "chat/chat-shared.h"
#include "contacts/contact-set.h"

#include "chat.h"

KaduSharedBaseClassImpl(Chat)

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Null Chat object.
 *
 * Null Chat object (without @link ChatShared @endlink attached).
 */
Chat Chat::null;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new empty Chat object.
 * @return new empty Chat object, with @link ChatShared @endlink attached.
 *
 * Creates new empty Chat object, with @link ChatShared @endlink attached.
 */
Chat Chat::create()
{
	return new ChatShared();
}

Chat Chat::loadStubFromStorage(const QSharedPointer<StoragePoint> &chatStoragePoint)
{
	return ChatShared::loadStubFromStorage(chatStoragePoint);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads Chat object from storage.
 * @param chatStoragePoint storage point from which chat data will be loaded
 * @return Chat object loaded from given storage poin.
 *
 * Loads Chat object from storage point and returns it. Real data will be loaded
 * at first usage of Chat object.
 */
Chat Chat::loadFromStorage(const QSharedPointer<StoragePoint> &chatStoragePoint)
{
	return ChatShared::loadFromStorage(chatStoragePoint);
}

Chat::Chat()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns new access object for given ChatShared instance.
 * @return new access object for given ChatShared instance
 *
 * Returns new access object for given ChatShared instance.
 */
Chat::Chat(ChatShared *data) :
		SharedBase<ChatShared>(data)
{
	data->ref.ref();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Casts QObject * to Chat class.
 * @param data QObject * instance that could be casted to Chat object
 * @return Chat object from given data object or Chat::null if data cannot be cast to Chat.
 *
 * If data parameter contains object of type ChatShared this method will create access
 * object to this data and return it. Else, Chat::null will be returned.
 */
Chat::Chat(QObject *data)
{
	ChatShared *shared = dynamic_cast<ChatShared *>(data);
	if (shared)
		setData(shared);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Copy contructor for other Chat object.
 * @param copy Chat object to copy
 * @return Chat object that will be access object for the same data as copy object.
 *
 * Creates new Chat object that will be access object fot he same data as copy object.
 */
Chat::Chat(const Chat &copy) :
		SharedBase<ChatShared>(copy)
{
}

Chat::~Chat()
{
}

KaduSharedBase_PropertyReadDef(Chat, ContactSet, contacts, Contacts, ContactSet())
KaduSharedBase_PropertyReadDef(Chat, QString, name, Name, QString())
KaduSharedBase_PropertyDef(Chat, ChatDetails *, details, Details, 0)
KaduSharedBase_PropertyDef(Chat, Account, chatAccount, ChatAccount, Account::null)
KaduSharedBase_PropertyDef(Chat, QString, type, Type, QString())
KaduSharedBase_PropertyBoolDef(Chat, IgnoreAllMessages, false)
