/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_TYPE_MANAGER
#define CHAT_TYPE_MANAGER

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

#include "chat/type/chat-type.h"
#include "exports.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeManager
 * @author Rafal 'Vogel' Malinowski
 * @short Manager of registered chat types.
 *
 * This class holds information about all @link ChatType @endlink objects registered in
 * system. All changes in this manager are propagated to @link ChatTypeAwareObject @endlink.
 */
class KADUAPI ChatTypeManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeManager)

	static ChatTypeManager * Instance;

	QList<ChatType *> ChatTypes;
	QMap<QString, ChatType *> ChatTypesMap;

	ChatTypeManager();
	virtual ~ChatTypeManager();

	void init();

public:
	static ChatTypeManager * instance();

	void addChatType(ChatType *chatType);
	void removeChatType(ChatType *chatType);

	const QList<ChatType *> & chatTypes() const;
	ChatType * chatType(const QString &name);

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just before chat type is added to manager.
	 * @param chatType added chat type
	 *
	 * Signal is emitted just before chat type is added to manager.
	 */
	void chatTypeAboutToBeAdded(ChatType *chatType);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just after chat type is added to manager.
	 * @param chat added chat type
	 *
	 * Signal is emitted just after chat typeis added to manager.
	 */
	void chatTypeAdded(ChatType *chatType);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just before chat type is removed from manager.
	 * @param chat removed chat type
	 *
	 * Signal is emitted just before chat type is removed from manager.
	 */
	void chatTypeAboutToBeRemoved(ChatType *chatType);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just after chat type is removed from manager.
	 * @param chat removed chat type
	 *
	 * Signal is emitted just after chat type is removed from manager.
	 */
	void chatTypeRemoved(ChatType *chatType);

};

/**
 * @}
 */

#endif // CHAT_TYPE_MANAGER
