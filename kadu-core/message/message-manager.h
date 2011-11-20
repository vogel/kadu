/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "message/message.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Buddy;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @author Rafał 'Vogel' Malinowski
 * @class MessageManager
 * @short Manager of messages used in Kadu.
 *
 * This singleton class manages messages that go trought Kadu. It also stores all pending messages in permament storage.
 * Pending message is an incoming message that have not ever been displayed to user.
 */
class KADUAPI MessageManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	QList<Message> UnreadMessages;

	MessageManager();
	virtual ~MessageManager();

	void init();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Imports list of pending messages from < 0.10.0 configurations.
	 * @return true if there was something to import
	 *
	 * This methods import list of pending messages from < 0.10.0 configurations. If there was no such
	 * list false is returned.
	 */
	bool importFromPendingMessages();

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a new message was received from any of registered accounts.
	 * @param message received message
	 *
	 * This slot is called every time a new message was received from any of registered accounts.
	 * Each receied message is automatically recorded as unread message and can be set as pending message
	 * if no applicable chat widget is available.
	 *
	 * This slot emits messageReceived and unreadMessageAdded signals.
	 */
	void messageReceivedSlot(const Message &message);

protected:
	virtual void load();
	virtual void store();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static MessageManager * instance();

	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("Messages"); }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds new unread message to the list.
	 * @param message new unread message
	 *
	 * Adds new unread message to the list. Message's pending status is set to true if no applicable
	 * chat widget is found. Signal unreadMessageAdded is emited.
	 */
	void addUnreadMessage(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove unread message to the list.
	 * @param message unread message to remove
	 *
	 * Removes unread message to the list. Message's pending status is set to false.
	 * Signal unreadMessageRemoved is emited.
	 */
	void removeUnreadMessage(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of all unread messages.
	 * @return list of all unread messages
	 *
	 * Returns list of all unread messages.
	 */
	const QList<Message> & allUnreadMessages() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of all unread messages for given chat.
	 * @param chat chat to get unread messages for
	 * @return list of all unread messages for given chat
	 *
	 * Returns list of all unread messages for given chat.
	 */
	QList<Message> chatUnreadMessages(const Chat &chat) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if there is any unread message in manager.
	 * @return true if there is any unread message in manager
	 *
	 * Returns true if there is any unread message in manager.
	 */
	bool hasUnreadMessages() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns count of unread messages in manager.
	 * @return count of unread messages in manager
	 *
	 * Returns count of unread messages in manager.
	 */
	quint16 unreadMessagesCount() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Marks all unread messages of given chat as read.
	 * @param chat chat to operate on
	 *
	 * Marks all unread messages of given chat as read. Messages are removed from unread list, its statuses
	 * are updated to MessageStatusRead and its pending statues are set to false.
	 * Signal unreadMessageRemoved is emited for each changed message.
	 */
	void markAllMessagesAsRead(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns one of unread messages.
	 * @return one of unread messages
	 *
	 * Returns one of unread messages. If no unread messages are available, returns null message.
	 */
	Message unreadMessage() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns one of unread messages sent by given buddy.
	 * @param buddy sender of message
	 * @return one of unread messages sent by given buddy
	 *
	 * Returns one of unread messages sent by given buddy. If no applicable messages are available, returns null message.
	 */
	Message unreadMessageForBuddy(const Buddy &buddy) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns one of unread messages sent by given contact.
	 * @param contact sender of message
	 * @return one of unread messages sent by given contact
	 *
	 * Returns one of unread messages sent by given contact. If no applicable messages are available, returns null message.
	 */
	Message unreadMessageForContact(const Contact &contact) const;

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited every time a message is received from one of registered acocunts.
	 * @param message received message
	 *
	 * This signal is emited every time a message is received from one of registered acocunts. It is not emited
	 * if message is added to the system in any other way (e.g. by inserting it manually into chat widget).
	 */
	void messageReceived(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited every time a message is sent trought one of registered acocunts.
	 * @param message sent message
	 *
	 * This signal is emited every time a message is sent trought one of registered acocunts.
	 */
	void messageSent(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited every time an unread message is added to manager.
	 * @param message added unread message
	 *
	 * This signal is emited every time an unread message is added to manager.
	 */
	void unreadMessageAdded(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited every time an unread message is removed from manager.
	 * @param message removed unread message
	 *
	 * This signal is emited every time an unread message is removed from manager.
	 */
	void unreadMessageRemoved(const Message &message);

};

/**
 * @}
 */

#endif // MESSAGE_MANAGER_H
