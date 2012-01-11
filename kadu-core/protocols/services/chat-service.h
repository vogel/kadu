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
#include "message/formatted-message.h"

#include "exports.h"

#include "protocols/services/protocol-service.h"

class Message;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ChatService
 * @short Chat service allows sending and receiving messages.
 *
 * This service allows sending and receiving messages. Each message can be modified before sending or after
 * receiving to allow protocol-independent encryption and filtering.
 *
 * One method must be reimplemented by derivered sevices: sendMessage().
 */
class KADUAPI ChatService : public ProtocolService
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of ChatService bound to given Protocol.
	 * @param protocol protocol to bound this service to
	 */
	explicit ChatService(Protocol *protocol);
	virtual ~ChatService();

public slots:
	/**
	 * @short Send new message to given chat.
	 * @param chat chat to send message to
	 * @param message message to be sent
	 * @param silent if true, no messageSent signal will be emitted
	 *
	 * This methods sends a message to given chat. Message is passed as HTML string. Protocols are
	 * free to ignore any HTML formatting.
	 *
	 * If silent parameter is true, no messageSent signal will be emitted. This is usefull for plugins
	 * like firewall or for sending public keys, as messageSent is usually used to add sent message to
	 * chat view.
	 */
	virtual bool sendMessage(const Chat &chat, const QString &message, bool silent = false) = 0;

signals:
	/**
	 * @short Signal emitted before message is sent to peer.
	 * @param chat chat that will receive this message
	 * @param message message in raw format
	 * @param stop flag used to stop sending a message
	 *
	 * This signal must be emitted by any implementation of ChatService before message is converted to UTF8 form and
	 * sent. Objects connected to this signal can modify message in any way (for example, encrypt it) or stop sending
	 * by setting stop flag to true. This signal is emitted before filterOutgoingMessage().
	 *
	 * If sending is stopped, no messageSent() signal can be emitted.
	 */
	void filterRawOutgoingMessage(Chat chat, QByteArray &message, bool &stop);

	/**
	 * @short Signal emitted before message is sent to peer.
	 * @param chat chat that will receive this message
	 * @param message message in UTF8 format
	 * @param stop flag used to stop sending a message
	 *
	 * This signal must be emitted by any implementation of ChatService after message is converted to UTF8 form and
	 * before it is sent. Objects connected to this signal can modify message in any way or stop sending
	 * by setting stop flag to true. This signal is emitted after filterRawOutgoingMessage().
	 *
	 * If sending is stopped, no messageSent() signal can be emitted.
	 */
	void filterOutgoingMessage(Chat chat, QString &message, bool &stop);

	/**
	 * @short Signal emitted after message is received from peer.
	 * @param chat chat that sent this message
	 * @param sender contact that sent this message
	 * @param message message in raw format
	 * @param ignore flag used to prevent messageReceived signal from being emitted
	 *
	 * This signal must be emitted by any implementation of ChatService after message is received from peer but before
	 * it is converted to UTF8 form and delivered to other parts of application. Objects connected to this signal can
	 * modify message in any way (for example, decrypt it) or ignore it by setting ignore flag to true. This signal is
	 * emitted before filterIncomingMessage().
	 *
	 * If ignore is set to true, no messageReceived() signal will be emitted.
	 */
	void filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore);

	/**
	 * @short Signal emitted after message is received from peer.
	 * @param chat chat that sent this message
	 * @param sender contact that sent this message
	 * @param message message in UTF8 format
	 * @param ignore flag used to prevent messageReceived signal from being emitted
	 *
	 * This signal must be emitted by any implementation of ChatService after message is received from peer and after
	 * it is converted to UTF8 form but before it is delivered to other parts of application. Objects connected to this
	 * ignal can modify message in any way  or ignore it by setting ignore flag to true. This signal is emitted after
	 * filterRawIncomingMessage().
	 *
	 * If ignore is set to true, no messageReceived() signal will be emitted.
	 */
	void filterIncomingMessage(Chat chat, Contact sender, QString &message, bool &ignore);

	/**
	 * @short Signal emitted when sent message status has changed.
	 * @param message message with changed status
	 *
	 * This signal is emitted every time a protocol learns about delivery status of sent message.
	 */
	void sentMessageStatusChanged(const Message &message);

	/**
	 * @short Signal emitted when message is sent.
	 * @param message sent message
	 *
	 * This signal is emitted every message is sent (in non-silent mode). This message can be marked as
	 * delivered or pending, depending on protocol.
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
