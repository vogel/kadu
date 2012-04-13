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

#include "chat/chat-details-buddy.h"
#include "chat/chat.h"
#include "icons/kadu-icon.h"

#include "chat-type-buddy.h"

ChatTypeBuddy::ChatTypeBuddy(QObject *parent) :
		ChatType(parent)
{
	Aliases.append("Buddy");
	Aliases.append("Aggregate");
}

ChatTypeBuddy::~ChatTypeBuddy()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeBuddy is 'Buddy'.
 */
QString ChatTypeBuddy::name() const
{
	return "Buddy";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal aliases of chat type.
 * @return internal aliases of chat type
 *
 * Chat type internal aliases, used to imporitng old configurations.
 *
 * Internal aliases for ChatTypeBuddy are 'Buddy' and 'Aggregate'.
 */
QStringList ChatTypeBuddy::aliases() const
{
	return Aliases;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeBuddy is 'kadu_icons/conference'.
 */
KaduIcon ChatTypeBuddy::icon() const
{
	return KaduIcon("kadu_icons/conference");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Window role for this chat type.
 * @return window role for this chat type.
 *
 * Kadu window role for this chat type.
 * For aggregate the role is "kadu-chat-aggregate".
 */
QString ChatTypeBuddy::windowRole() const
{
	return "kadu-chat-aggregate";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsBuddy object for given chat type.
 * @return new ChatDetailsBuddy object for given chat type
 *
 * Creates new @link ChatDetailsBuddy @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeBuddy::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsBuddy(chatData);
}

ChatEditWidget * ChatTypeBuddy::createEditWidget(const Chat &chat, QWidget *parent) const
{
	Q_UNUSED(chat);
	Q_UNUSED(parent);

	return 0;
}
