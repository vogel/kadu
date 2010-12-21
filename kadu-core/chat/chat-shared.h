/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

#include "accounts/account.h"
#include "chat/type/chat-type-aware-object.h"
#include "storage/details-holder.h"
#include "storage/shared.h"

class BuddySet;
class Chat;
class ChatDetails;
class ChatManager;
class ContactSet;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatShared
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data describing object.
 *
 * This class contains standard data that are common to all chat types used in application.
 * Data specific to given chat type is stored in classes derivered from @link ChatDetails @endlink
 * and attached to objects of this class using @link setDetails @endlink and @link details @endlink
 * methods of @link DetailsHolder @endlink class.
 */
class KADUAPI ChatShared : public QObject, public Shared, public DetailsHolder<ChatShared, ChatDetails, ChatManager>, ChatTypeAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatShared)

	Account ChatAccount;
	QString Type;
	bool IgnoreAllMessages;

protected:
	virtual void load();

	void emitUpdated();

	virtual void chatTypeRegistered(ChatType *chatType);
	virtual void chatTypeUnregistered(ChatType *chatType);

	virtual void detailsAdded();
	virtual void detailsAboutToBeRemoved();

public:
	static ChatShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint);
	static ChatShared * loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint);

	explicit ChatShared(QUuid uuid = QUuid::createUuid());
	virtual ~ChatShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	ContactSet contacts();
	QString name();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Account of this chat.
	 *
	 * Every chat is assigned to account. All contacts in every chat must
	 * belong to the same account as chat.
	 */
	KaduShared_Property(Account, chatAccount, ChatAccount);

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
	KaduShared_Property(QString, type, Type);

	// temporary, not stored, lost after program close
	KaduShared_PropertyBool(IgnoreAllMessages)

signals:
	void updated();

};

/**
 * @}
 */

#include "chat/chat.h" // for MOC

#endif // CHAT_SHARED_H
