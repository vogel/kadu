/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-details-conference.h"
#include "icons/kadu-icon.h"

#include "chat-type-conference.h"

ChatTypeConference::ChatTypeConference(QObject *parent) :
		ChatType(parent)
{
}

ChatTypeConference::~ChatTypeConference()
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
 * Sort index for ChatTypeConference is 1.
 */
int ChatTypeConference::sortIndex() const
{
	return 1;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeConference is 'Conference'.
 */
QString ChatTypeConference::name() const
{
	return "Conference";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Display name of chat type.
 * @return display name of chat type
 *
 * Chat type display name. Display name is used in history window. This name
 * is translated into national languaes.
 *
 * Display name for ChatTypeConference is 'Conference'.
 */
QString ChatTypeConference::displayName() const
{
	return tr("Conference");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Display name of chat type in plural form.
 * @return display name of chat typein plural form
 *
 * Chat type display name in plural form. Display name is used in history window.
 * This name is translated into national languaes.
 *
 * Plural display name for ChatTypeConference is 'Conferences'.
 */
QString ChatTypeConference::displayNamePlural() const
{
	return tr("Conferences");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeConference is 'kadu_icons/conference'.
 */
KaduIcon ChatTypeConference::icon() const
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
QString ChatTypeConference::windowRole() const
{
	return "kadu-chat-conference";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsConference object for given chat type.
 * @return new ChatDetailsConference object for given chat type
 *
 * Creates new @link ChatDetailsConference @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeConference::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsConference(chatData);
}
