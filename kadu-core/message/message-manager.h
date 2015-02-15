/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

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
 * This singleton class manages messages that go trought Kadu. It also stores all pending messages in permanent storage.
 * Pending message is an incoming message that have not ever been displayed to user.
 */
class KADUAPI MessageManager : public QObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	QPointer<MessageFilterService> CurrentMessageFilterService;
	QPointer<MessageTransformerService> CurrentMessageTransformerService;
	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	MessageManager();
	virtual ~MessageManager();

	/**
	 * @short Create outoing message for given chat and given content.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of outgoing message
	 * @param content content of outgoing message
	 */
	Message createOutgoingMessage(const Chat &chat, std::unique_ptr<FormattedString> &&content);

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
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static MessageManager * instance();

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
	 */
	bool sendMessage(const Chat &chat, std::unique_ptr<FormattedString> &&content, bool silent = false);

	/**
	 * @short Send new raw message to given chat.
	 * @param chat chat to send message to
	 * @param content raw content to be sent
	 *
	 * This methods sends a message to given chat. Message is passed as QByteArray.
	 *
	 * Raw messages will not invoke messageSent signals.
	 */
	bool sendRawMessage(const Chat &chat, const QByteArray &content);

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

};

/**
 * @}
 */

#endif // MESSAGE_MANAGER_H
