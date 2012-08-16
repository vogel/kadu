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
class FormattedStringFactory;
class MessageFilterService;
class MessageTransformerService;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageManager
 * @short Manager of messages used in Kadu.
 * @author Rafał 'Vogel' Malinowski
 *
 * This singleton class manages messages that go trought Kadu. It also stores all pending messages in permament storage.
 * Pending message is an incoming message that have not ever been displayed to user.
 */
class KADUAPI MessageManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	QWeakPointer<MessageFilterService> CurrentMessageFilterService;
	QWeakPointer<MessageTransformerService> CurrentMessageTransformerService;
	QWeakPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	QList<Message> UnreadMessages;

	MessageManager();
	virtual ~MessageManager();

	void init();

	/**
	 * @short Create outoing message for given chat and given content.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of outgoing message
	 * @param content content of outgoing message
	 */
	Message createOutgoingMessage(const Chat &chat, FormattedString *content);

	/**
	 * @short Imports list of pending messages from < 0.10.0 configurations.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if there was something to import
	 *
	 * This methods import list of pending messages from < 0.10.0 configurations. If there was no such
	 * list false is returned.
	 */
	bool importFromPendingMessages();

private slots:
	/**
	 * @short Slot called every time a new message was received from any of registered accounts.
	 * @author Rafał 'Vogel' Malinowski
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
	 * @short Set message filter service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageFilterService message filter service for this service
	 */
	void setMessageFilterService(MessageFilterService *messageFilterService);

	/**
	 * @short Set message transformer service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageTransformerService message transformer service for this service
	 */
	void setMessageTransformerService(MessageTransformerService *messageTransformerService);

	/**
	 * @short Set formatted string factory for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param formattedStringFactory formatted string factory for this service
	 */
	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	/**
	 * @short Send new message to given chat.
	 * @param chat chat to send message to
	 * @param content content to be sent, can be HTML or plain text
	 * @param silent if true, no messageSent signal will be emitted
	 *
	 * This methods sends a message to given chat. Message is passed as HTML string. Protocols are
	 * free to ignore any HTML formatting.
	 *
	 * If silent parameter is true, no messageSent signal will be emitted. This is usefull for plugins
	 * like firewall or for sending public keys, as messageSent is usually used to add sent message to
	 * chat view.
	 */
	bool sendMessage(const Chat &chat, const QString &content, bool silent = false);

	/**
	 * @short Send new message to given chat.
	 * @param chat chat to send message to
	 * @param content formatted content to be sent
	 * @param silent if true, no messageSent signal will be emitted
	 *
	 * This methods sends a message to given chat. Message is passed as HTML string. Protocols are
	 * free to ignore any HTML formatting.
	 *
	 * If silent parameter is true, no messageSent signal will be emitted. This is usefull for plugins
	 * like firewall or for sending public keys, as messageSent is usually used to add sent message to
	 * chat view.
	 *
	 * This method takes ownership of content parameter and deletes it at the end.
	 */
	bool sendMessage(const Chat &chat, FormattedString *content, bool silent = false);

	/**
	 * @short Adds new unread message to the list.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message new unread message
	 *
	 * Adds new unread message to the list. Message's pending status is set to true if no applicable
	 * chat widget is found. Signal unreadMessageAdded is emited.
	 */
	void addUnreadMessage(const Message &message);

	/**
	 * @short Remove unread message to the list.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message unread message to remove
	 *
	 * Removes unread message to the list. Message's pending status is set to false.
	 * Signal unreadMessageRemoved is emited.
	 */
	void removeUnreadMessage(const Message &message);

	/**
	 * @short Returns list of all unread messages.
	 * @author Rafał 'Vogel' Malinowski
	 * @return list of all unread messages
	 *
	 * Returns list of all unread messages.
	 */
	const QList<Message> & allUnreadMessages() const;

	/**
	 * @short Returns list of all unread messages for given chat.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat to get unread messages for
	 * @return list of all unread messages for given chat
	 *
	 * Returns list of all unread messages for given chat.
	 */
	QList<Message> chatUnreadMessages(const Chat &chat) const;

	/**
	 * @short Returns true if there is any unread message in manager.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if there is any unread message in manager
	 *
	 * Returns true if there is any unread message in manager.
	 */
	bool hasUnreadMessages() const;

	/**
	 * @short Returns count of unread messages in manager.
	 * @author Rafał 'Vogel' Malinowski
	 * @return count of unread messages in manager
	 *
	 * Returns count of unread messages in manager.
	 */
	quint16 unreadMessagesCount() const;

	/**
	 * @short Marks all unread messages of given chat as read.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat to operate on
	 *
	 * Marks all unread messages of given chat as read. Messages are removed from unread list, its statuses
	 * are updated to MessageStatusRead and its pending statues are set to false.
	 * Signal unreadMessageRemoved is emited for each changed message.
	 */
	void markAllMessagesAsRead(const Chat &chat);

	/**
	 * @short Returns one of unread messages.
	 * @author Rafał 'Vogel' Malinowski
	 * @return one of unread messages
	 *
	 * Returns one of unread messages. If no unread messages are available, returns null message.
	 */
	Message unreadMessage() const;

	/**
	 * @short Returns one of unread messages sent by given buddy.
	 * @author Rafał 'Vogel' Malinowski
	 * @param buddy sender of message
	 * @return one of unread messages sent by given buddy
	 *
	 * Returns one of unread messages sent by given buddy. If no applicable messages are available, returns null message.
	 */
	Message unreadMessageForBuddy(const Buddy &buddy) const;

	/**
	 * @short Returns one of unread messages sent by given contact.
	 * @author Rafał 'Vogel' Malinowski
	 * @param contact sender of message
	 * @return one of unread messages sent by given contact
	 *
	 * Returns one of unread messages sent by given contact. If no applicable messages are available, returns null message.
	 */
	Message unreadMessageForContact(const Contact &contact) const;

signals:
	/**
	 * @short Signal emited every time a message is received from one of registered acocunts.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message received message
	 *
	 * This signal is emited every time a message is received from one of registered acocunts. It is not emited
	 * if message is added to the system in any other way (e.g. by inserting it manually into chat widget).
	 */
	void messageReceived(const Message &message);

	/**
	 * @short Signal emited every time a message is sent trought one of registered acocunts.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message sent message
	 *
	 * This signal is emited every time a message is sent trought one of registered acocunts.
	 */
	void messageSent(const Message &message);

	/**
	 * @short Signal emited every time an unread message is added to manager.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message added unread message
	 *
	 * This signal is emited every time an unread message is added to manager.
	 */
	void unreadMessageAdded(const Message &message);

	/**
	 * @short Signal emited every time an unread message is removed from manager.
	 * @author Rafał 'Vogel' Malinowski
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
