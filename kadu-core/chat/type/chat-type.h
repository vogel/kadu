/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "exports.h"

class Chat;
class ChatDetails;
class ChatEditWidget;
class ChatShared;
class KaduIcon;

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
	 * @short Internal name of chat type.
	 * @return internal name of chat type
	 *
	 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
	 * and also it is stored with @link Chat @endlink data.
	 */
	virtual QString name() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Internal aliases of chat type.
	 * @return internal aliases of chat type
	 *
	 * Chat type internal aliases. Used to import from old confirations where Simple was used
	 * instead of Contact and Conference instead of ContactSet.
	 */
	virtual QStringList aliases() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Icon of chat type.
	 * @return icon of chat type
	 *
	 * Chat type icon. Icon is used in history window and as icon of chat windows.
	 */
	virtual KaduIcon icon() const = 0;

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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Create new ChatEditWidget for editing given chat.
	 * @param chat chat to edit
	 * @param parent QWidget parent of new edit widget
	 * @return ChatEditWidget for editing given chat
	 *
	 * May return null.
	 */
	virtual ChatEditWidget * createEditWidget(const Chat &chat, QWidget *parent) const = 0;

};

Q_DECLARE_METATYPE(ChatType *)

/**
 * @}
 */

#endif // CHAT_TYPE_H
