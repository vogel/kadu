/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-type-manager.h"

#include "chat/type/chat-type-buddy.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-room.h"
#include "icons/icons-manager.h"

ChatTypeManager::ChatTypeManager(QObject *parent) : QObject{parent}
{
}

ChatTypeManager::~ChatTypeManager()
{
}

void ChatTypeManager::setChatTypeBuddy(ChatTypeBuddy *chatTypeBuddy)
{
    m_chatTypes.append(chatTypeBuddy);
}

void ChatTypeManager::setChatTypeContactSet(ChatTypeContactSet *chatTypeContactSet)
{
    m_chatTypes.append(chatTypeContactSet);
}

void ChatTypeManager::setChatTypeContact(ChatTypeContact *chatTypeContact)
{
    m_chatTypes.append(chatTypeContact);
}

void ChatTypeManager::setChatTypeRoom(ChatTypeRoom *chatTypeRoom)
{
    m_chatTypes.append(chatTypeRoom);
}

/**
 * @short Returns chat type with given internal alias.
 * @param name internal alias of chat type to return.
 * @return chat type with given internal alias
 *
 * Returns chat type with given internal alias or null, if not found.
 */
ChatType *ChatTypeManager::chatType(const QString &alias) const
{
    for (auto chatType : m_chatTypes)
        for (auto const &chatTypeAlias : chatType->aliases())
            if (alias == chatTypeAlias)
                return chatType;
    return nullptr;
}

#include "moc_chat-type-manager.cpp"
