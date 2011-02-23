/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CHAT_TYPE_H
#define CHAT_TYPE_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

#include "exports.h"

class ChatDetails;
class ChatShared;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatType
 * @author Rafal 'Vogel' Malinowski
 * @short Representation of logical chat type.
 *
 * Abstract representation of logical chat type (single chat, conference chat, irc room chat...).
 * It consists of internal name, display name, icon and sort index (priority).
 *
 * Concrete chat types are implemented by sublasses of this class.
 */
class KADUAPI ChatType : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatType)

public:
	explicit ChatType(QObject *parent = 0);
	virtual ~ChatType();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sort index for this object.
	 * @return sort index for this object
	 *
	 * Sort index is used for sorting chat types in history window.
	 * Chats with smaller sort indexes are displayed first.
	 */
	virtual int sortIndex() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Internal name of chat type.
	 * @return internal name of chat type
	 *
	 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
	 * and also it is stored with @link Chat @endlink data.
	 */
	virtual QString name() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Display name of chat type.
	 * @return display name of chat type
	 *
	 * Chat type display name. Display name is used in history window. This name
	 * is translated into national languaes.
	 */
	virtual QString displayName() const = 0;

	/**
	 * @author Piotr 'ultr' Dąbrowski
	 * @short Display name of chat type in plural form.
	 * @return display name of chat type in plural form
	 *
	 * Plural chat type display name. Display name is used in history window.
	 * This name is translated into national languaes.
	 */
	virtual QString displayNamePlural() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Icon of chat type.
	 * @return icon of chat type
	 *
	 * Chat type icon. Icon is used in history window and as icon of chat windows.
	 */
	virtual QIcon icon() const = 0;

	/**
	 * @author Piotr 'ultr' Dąbrowski
	 * @short Window role for this chat type.
	 * @return window role for this chat type.
	 *
	 * Kadu window role for this chat type: "kadu-chat-...".
	 */
	virtual QString windowRole() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Creates new ChatDetails object for given chat type.
	 * @return new ChatDetails object for given chat type
	 *
	 * Creates new @link ChatDetails @endlink object for given chat type and for
	 * given @link Chat @endlink (@link ChatShared @endlink).
	 */
	virtual ChatDetails * createChatDetails(ChatShared *chatData) const = 0;

	bool operator == (const ChatType &compare) const;
	bool operator < (const ChatType &compare) const;

};

Q_DECLARE_METATYPE(ChatType *)

/**
 * @}
 */

#endif // CHAT_TYPE_H
