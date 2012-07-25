/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include "buddies/buddy-list.h"

#include "chat/chat.h"

#include "exports.h"

#include "protocols/services/account-service.h"

class FormattedString;
class Message;
class MessageFilterService;
class MessageTransformerService;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ChatService
 * @short Chat service allows sending and receiving messages.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service allows sending and receiving messages. Each message can be modified before sending or after
 * receiving to allow protocol-independent encryption and filtering.
 *
 * One method must be reimplemented by derivered sevices: sendMessage().
 */
class KADUAPI ChatService : public AccountService
{
	Q_OBJECT

	QWeakPointer<MessageFilterService> CurrentMessageFilterService;
	QWeakPointer<MessageTransformerService> CurrentMessageTransformerService;

protected:
	explicit ChatService(Account account, QObject *parent = 0);
	virtual ~ChatService();

public:
	/**
	 * @short Set message filter service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageFilterService message filter service for this service
	 */
	void setMessageFilterService(MessageFilterService *messageFilterService);

	/**
	 * @short Return message filter service of this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return message filter service of this service
	 */
	MessageFilterService * messageFilterService() const;

	/**
	 * @short Set message transformer service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageTransformerService message transformer service for this service
	 */
	void setMessageTransformerService(MessageTransformerService *messageTransformerService);

	/**
	 * @short Return message transformer service of this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return message transformer service of this service
	 */
	MessageTransformerService * messageTransformerService() const;

public slots:
	/**
	 * @short Send new message to given chat.
	 * @param chat chat to send message to
	 * @param message message to be sent
	 *
	 * This methods sends a message to given chat. Message is passed as HTML string. Protocols are
	 * free to ignore any HTML formatting.
	 *
	 * If silent parameter is true, no messageSent signal will be emitted. This is usefull for plugins
	 * like firewall or for sending public keys, as messageSent is usually used to add sent message to
	 * chat view.
	 */
	virtual bool sendMessage(const Chat &chat, const Message &message, const FormattedString &formattedString, const QString &plain) = 0;

signals:
	/**
	 * @short Signal emitted when sent message status has changed.
	 * @param message message with changed status
	 *
	 * This signal is emitted every time a protocol learns about delivery status of sent message.
	 */
	void sentMessageStatusChanged(const Message &message);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emitted when message was sent.
	 * @param message sent message
	 *
	 * This signal is emited every time a message is sent trought one of registered acocunts.
	 */
	void messageSent(const Message &message);

	/**
	 * @short Signal emitted when message is received.
	 * @param message received message
	 *
	 * This signal is emitted every message is received and not ignored.
	 */
	void messageReceived(const Message &message);

};

/**
 * @}
 */

#endif // CHAT_SERVICE_H
