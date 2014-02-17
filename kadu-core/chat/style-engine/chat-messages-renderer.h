/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message.h"
#include "protocols/services/chat-state-service.h"

#include <QtCore/QVector>

class ChatImage;

class QWebFrame;

class ChatMessagesRenderer
{

public:
	virtual ~ChatMessagesRenderer() {}

	virtual void clearMessages(QWebFrame &frame) = 0;
	virtual void appendMessages(QWebFrame &frame, const Chat &chat, const QVector<Message> &newMessages, const Message &lastMessage, const QVector<Message> &allMessages, bool pruneEnabled) = 0;
	virtual void appendMessage(QWebFrame &frame, const Chat &chat, const Message &newMessage, const Message &lastMessage, const QVector<Message> &allMessages, bool pruneEnabled) = 0;
	virtual void pruneMessage(QWebFrame &frame) = 0;
	virtual void refreshView(QWebFrame &frame, const Chat &chat, const QVector<Message> &allMessages, bool useTransparency = false) = 0;
	virtual void messageStatusChanged(QWebFrame &frame, Message, MessageStatus) = 0;
	virtual void contactActivityChanged(QWebFrame &frame, ChatStateService::State, const QString &, const QString &) = 0;
	virtual void chatImageAvailable(QWebFrame &frame, const ChatImage &chatImage, const QString &fileName) = 0;

};
