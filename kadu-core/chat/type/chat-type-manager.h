/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "chat/type/chat-type.h"
#include "exports.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatTypeBuddy;
class ChatTypeContactSet;
class ChatTypeContact;
class ChatTypeRoom;

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

private:
	QPointer<ChatTypeBuddy> m_chatTypeBuddy;
	QPointer<ChatTypeContactSet> m_chatTypeContactSet;
	QPointer<ChatTypeContact> m_chatTypeContact;
	QPointer<ChatTypeRoom> m_chatTypeRoom;

	QList<ChatType *> ChatTypes;
	QMap<QString, ChatType *> ChatTypesMap;

public:
	Q_INVOKABLE explicit ChatTypeManager(QObject *parent = nullptr);
	virtual ~ChatTypeManager();

	void registerChatType(ChatType *chatType);
	void unregisterChatType(ChatType *chatType);

	const QList<ChatType *> & chatTypes() const;
	ChatType * chatType(const QString &alias);

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

private slots:
	INJEQT_SET void setChatTypeBuddy(ChatTypeBuddy *chatTypeBuddy);
	INJEQT_SET void setChatTypeContactSet(ChatTypeContactSet *chatTypeContactSet);
	INJEQT_SET void setChatTypeContact(ChatTypeContact *chatTypeContact);
	INJEQT_SET void setChatTypeRoom(ChatTypeRoom *chatTypeRoom);
	INJEQT_INIT void init();

};

/**
 * @}
 */
