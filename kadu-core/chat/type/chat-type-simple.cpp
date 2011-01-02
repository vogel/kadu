/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "chat/chat-details-simple.h"
#include "icons-manager.h"

#include "chat-type-simple.h"

ChatTypeSimple::ChatTypeSimple(QObject *parent) :
		ChatType(parent)
{
}

ChatTypeSimple::~ChatTypeSimple()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Sort index for this object.
 * @return sort index for this object
 *
 * Sort index is used for sorting chat types in history window.
 * Chats with smaller sort indexes are displayed first.
 *
 * Sort index for ChatTypeSimple is 0.
 */
int ChatTypeSimple::sortIndex() const
{
	return 0;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeSimple is 'Simple'.
 */
QString ChatTypeSimple::name() const
{
	return "Simple";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Display name of chat type.
 * @return display name of chat type
 *
 * Chat type display name. Display name is used in history window. This name
 * is translated into national languaes.
 *
 * Display name for ChatTypeSimple is 'Chat'.
 */
QString ChatTypeSimple::displayName() const
{
	return tr("Chat");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Display name of chat type in plural form.
 * @return display name of chat typein plural form
 *
 * Chat type display name in plural form. Display name is used in history window.
 * This name is translated into national languaes.
 *
 * Plural display name for ChatTypeSimple is 'Chats'.
 */
QString ChatTypeSimple::displayNamePlural() const
{
	return tr("Chats");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeSimple is 'internet-group-chat'.
 */
QIcon ChatTypeSimple::icon() const
{
	return IconsManager::instance()->iconByPath("internet-group-chat");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsSimple object for given chat type.
 * @return new ChatDetailsSimple object for given chat type
 *
 * Creates new @link ChatDetailsSimple @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeSimple::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsSimple(chatData);
}
