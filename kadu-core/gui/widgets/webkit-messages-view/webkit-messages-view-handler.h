/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "gui/widgets/webkit-messages-view/message-limiter.h"
#include "message/message.h"
#include "message/sorted-messages.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

#include <QtCore/QObject>

class ChatImage;
class ChatStyleRenderer;
class MessageLimiter;
class WebkitMessagesViewDisplay;
class WebkitMessagesViewDisplayFactory;

enum class MessageLimitPolicy;

class KADUAPI WebkitMessagesViewHandler : public QObject
{
	Q_OBJECT

public:
	explicit WebkitMessagesViewHandler(qobject_ptr<ChatStyleRenderer> chatStyleRenderer,
			std::unique_ptr<WebkitMessagesViewDisplay> messagesDisplay, QObject *parent = nullptr);
	virtual ~WebkitMessagesViewHandler();

	void setWebkitMessagesViewDisplayFactory(WebkitMessagesViewDisplayFactory *webkitMessagesViewDisplayFactory);

	void setMessageLimit(unsigned limit);
	void setMessageLimitPolicy(MessageLimitPolicy messageLimitPolicy);

	const SortedMessages & messages() const { return m_messages; }
	void add(const Message &message);
	void add(const SortedMessages &messages);
	void clear();

	void chatImageAvailable(const ChatImage &chatImage, const QString &fileName);
	void messageStatusChanged(const QString &id, MessageStatus status);
	void contactActivityChanged(const Contact &contact, ChatStateService::State state);

public slots:
	void refreshView();

private:
	QPointer<WebkitMessagesViewDisplayFactory> m_webkitMessagesViewDisplayFactory;

	qobject_ptr<ChatStyleRenderer> m_chatStyleRenderer;
	std::unique_ptr<WebkitMessagesViewDisplay> m_messagesDisplay;

	MessageLimiter m_messagesLimiter;
	SortedMessages m_messages;

	SortedMessages limitMessages(SortedMessages messages) const;
	void displayMessages(const SortedMessages &messages);

private slots:
	void rendererReady();

};
