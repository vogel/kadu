/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QSet>

#include "chat/type/chat-type-aware-object.h"
#include "contacts/contact.h"
#include "storage/shared.h"

class Account;
class BuddySet;
class Chat;
class ChatDetails;
class ContactSet;
class Group;

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
 * Data specific to given chat type is stored in classes derivered from @link ChatDetails @endlink..
 */
class KADUAPI ChatShared : public QObject, public Shared, ChatTypeAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatShared)

	Account *ChatAccount;
	ChatDetails *Details;

	QString Display;
	QString Type;
	bool IgnoreAllMessages;
	QSet<Group> Groups;
	quint16 UnreadMessagesCount;
	bool Open;

	bool doAddToGroup(const Group &group);
	bool doRemoveFromGroup(const Group &group);

private slots:
	void groupAboutToBeRemoved();

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

	virtual void chatTypeRegistered(ChatType *chatType);
	virtual void chatTypeUnregistered(ChatType *chatType);

public:
	static ChatShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);
	static ChatShared * loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);

	explicit ChatShared(const QUuid &uuid = QUuid::createUuid());
	virtual ~ChatShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	ChatDetails * details() const { return Details; }

	ContactSet contacts();
	QString name();

	KaduShared_PropertyRead(const QSet<Group> &, groups, Groups)
	void setGroups(const QSet<Group> &groups);
	bool showInAllGroup();
	bool isInGroup(const Group &group);
	void addToGroup(const Group &group);
	void removeFromGroup(const Group &group);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Account of this chat.
	 *
	 * Every chat is assigned to account. All contacts in every chat must
	 * belong to the same account as chat.
	 */
	KaduShared_PropertyDeclCRW(Account, chatAccount, ChatAccount)

	KaduShared_Property(const QString &, display, Display)

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Name of chat type.
	 *
	 * Name of chat type. @link ChatType @endlink object with the same name must be loaded
	 * and registered in @link ChatTypeManager @endlink to allow this chat object to
	 * be fully functional and used. Example chat types are: 'contact' (for one-to-one chats)
	 * and 'contact-set' (for on-to-many chats). Other what types could be: 'irc-room' (for irc room
	 * chats).
	 */
	KaduShared_PropertyWriteDecl(const QString &, type, Type)
	KaduShared_PropertyRead(const QString &, type, Type)

	// temporary, not stored, lost after program close
	KaduShared_PropertyBool(IgnoreAllMessages)

	KaduShared_Property(quint16, unreadMessagesCount, UnreadMessagesCount)

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Return true when chat is connected.
	 * @return true when chat is connected
	 *
	 * Chat messages can only be send to/received from connected chat.
	 * Chat connection depends on chat type and is implemented in @link ChatDetails @endlink subclasses.
	 *
	 * For example, simple Contact and ContactSet chats are connected when an account is connected.
	 * MUC chats in XMPP are connected when account is connected and given group chat is joined.
	 */
	bool isConnected() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Get value of Open property.
	 * @return true when chat is open
	 *
	 * Chat is open when an associated chat widget is open.
	 */
	bool isOpen() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set value of open property.
	 * @param open new value of Open property
	 *
	 * Changing value of Open property may result in emiting of @link opened() @endlink or @link closed() @endlink
	 * singals.
	 */
	void setOpen(bool open);

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when given chat has connected.
	 *
	 * Chat messages can only be send to/received from connected chat.
	 */
	void connected();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when given chat has disconnected.
	 *
	 * Chat messages can only be send to/received from connected chat.
	 */
	void disconnected();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when given chat has been opened.
	 */
	void opened();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when given chat has been closed.
	 */
	void closed();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited just before a new contact is added to this Chat.
	 * @param contact added contact
	 */
	void contactAboutToBeAdded(const Contact &contact);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when a new contact was added to this Chat.
	 * @param contact just added contact
	 */
	void contactAdded(const Contact &contact);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited just before a contact is removed from this Chat.
	 * @param contact removed contact
	 */
	void contactAboutToBeRemoved(const Contact &contact);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Signal emited when a contact was removed from this Chat.
	 * @param contact just removed contact
	 */
	void contactRemoved(const Contact &contact);

	void updated();

};

/**
 * @}
 */

#endif // CHAT_SHARED_H
