/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-details-aggregate.h"
#include "icons/kadu-icon.h"

#include "chat-type-aggregate.h"

ChatTypeAggregate::ChatTypeAggregate(QObject *parent) :
		ChatType(parent)
{
}

ChatTypeAggregate::~ChatTypeAggregate()
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
 * Sort index for ChatTypeAggregate is -1. That means, that this chat type wont be displayed in history window.
 */
int ChatTypeAggregate::sortIndex() const
{
	return -1;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeAggregate is 'Aggregate'.
 */
QString ChatTypeAggregate::name() const
{
	return "Aggregate";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Display name of chat type.
 * @return display name of chat type
 *
 * Chat type display name. Display name is used in history window. This name
 * is translated into national languaes.
 *
 * Display name for ChatTypeAggregate is 'Aggregate'.
 */
QString ChatTypeAggregate::displayName() const
{
	return tr("Aggregate");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Display name of chat type in plural form.
 * @return display name of chat typein plural form
 *
 * Chat type display name in plural form. Display name is used in history window.
 * This name is translated into national languaes.
 *
 * Plural display name for ChatTypeAggregate is 'Aggregate'.
 */
QString ChatTypeAggregate::displayNamePlural() const
{
	return tr("Aggregates");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeAggregate is 'kadu_icons/conference'.
 */
KaduIcon ChatTypeAggregate::icon() const
{
	return KaduIcon("kadu_icons/conference");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Window role for this chat type.
 * @return window role for this chat type.
 *
 * Kadu window role for this chat type.
 * For conference the role is "kadu-chat-conference".
 */
QString ChatTypeAggregate::windowRole() const
{
	return "kadu-chat-aggregate";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsConference object for given chat type.
 * @return new ChatDetailsConference object for given chat type
 *
 * Creates new @link ChatDetailsAggregate @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeAggregate::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsAggregate(chatData);
}
