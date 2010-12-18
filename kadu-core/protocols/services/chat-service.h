/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QObject>

#include "buddies/buddy-list.h"

#include "chat/chat.h"
#include "chat/message/formatted-message.h"

#include "exports.h"

class Message;

class KADUAPI ChatService : public QObject
{
	Q_OBJECT

public:
	enum MessageStatus {
		StatusAcceptedDelivered,
		StatusAcceptedQueued,
		StatusRejectedBlocked,
		StatusRejectedBoxFull,
		StatusRejectedUnknown
	};

	ChatService(QObject *parent = 0)
		: QObject(parent) {}

public slots:
	virtual bool sendMessage(Chat chat, const QString &messageContent, bool silent = false);
	virtual bool sendMessage(Chat chat, FormattedMessage &message, bool silent = false) = 0;

signals:
	void filterRawOutgoingMessage(Chat chat, QByteArray &message, bool &stop);
	void filterOutgoingMessage(Chat chat, QString &message, bool &stop);

	void filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore);
	void filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore);

	void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
	void messageSent(const Message &message);
	void messageReceived(const Message &message);

};

#endif // CHAT_SERVICE_H
