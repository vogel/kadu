/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CHAT_H
#define CHAT_H

#include "chat/chat-shared.h"
#include "storage/shared-base.h"
#include "exports.h"

class Account;
class ContactSet;
class ChatDetails;
class ChatType;
class StoragePoint;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class Chat
 * @author Rafal 'Vogel' Malinowski
 * @short Access object for chat data.
 *
 * This class allows to access to chat data defined in @link ChatShared @endlink class.
 */
class KADUAPI Chat : public SharedBase<ChatShared>
{
	KaduSharedBaseClass(Chat)

public:
	static Chat create();
	static Chat loadStubFromStorage(const QSharedPointer<StoragePoint> &chatStoragePoint);
	static Chat loadFromStorage(const QSharedPointer<StoragePoint> &chatStoragePoint);
	static Chat null;

	Chat();
	Chat(ChatShared *data);
	Chat(QObject *data);
	Chat(const Chat &copy);

	virtual ~Chat();

	ContactSet contacts() const;
	QString name() const;

	/**
	* @author Rafal 'Vogel' Malinowski
	* @short Details object for this chat.
	*
	* When ChatType for this chat is loaded and registered in ChatTypeManager
	* this field contains ChatDetails object that holds detailed information
	* about this chat.
	*/
	KaduSharedBase_Property(ChatDetails *, details, Details);

	/**
	* @author Rafal 'Vogel' Malinowski
	* @short Account of this chat.
	*
	* Every chat is assigned to account. All contacts in every chat must
	* belong to the same account as chat.
	*/
	KaduSharedBase_Property(Account, chatAccount, ChatAccount);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Name of chat type.
	 *
	 * Name of chat type. @link ChatType @endlink object with the same name must be loaded
	 * and registered in @link ChatTypeManager @endlink to allow this chat object to
	 * be fully functional and used. Example chat types are: 'simple' (for one-to-one chats)
	 * and 'conference' (for on-to-many chats). Other what types could be: 'irc-room' (for irc room
	 * chats).
	 */
	KaduSharedBase_Property(QString, type, Type);

	KaduSharedBase_PropertyBool(IgnoreAllMessages);

};

/**
 * @}
 */

Q_DECLARE_METATYPE(Chat)

#endif // CHAT_H
