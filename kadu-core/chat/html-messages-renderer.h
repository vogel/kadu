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

#include "chat/chat.h"
#include "message/message.h"
#include "protocols/services/chat-state-service.h"

#include <QtCore/QObject>

class ChatImage;
class ChatMessagesRenderer;

class QWebPage;

class HtmlMessagesRenderer : public QObject
{
	Q_OBJECT

public:
	explicit HtmlMessagesRenderer(Chat chat, QWebPage *parent = nullptr);
	virtual ~HtmlMessagesRenderer();

	Chat  chat() { return m_chat; }
	void setChat(Chat chat);

	QWebPage * webPage() const;

	bool pruneEnabled();

	bool forcePruneDisabled() { return m_forcePruneDisabled; }
	void setForcePruneDisabled(bool forcePruneDisabled);

	QString content();

	const QVector<Message> & messages() const { return m_messages; }
	void appendMessage(const Message &message);
	void appendMessages(const QVector<Message> &messages);
	void clearMessages();

	Message lastMessage() { return m_lastMessage; }
	void setLastMessage(Message message);

	void refresh();
	void refreshView(bool useTransparency);
	void chatImageAvailable(const ChatImage &chatImage, const QString &fileName);
	void updateBackgroundsAndColors();
	void messageStatusChanged(Message message, MessageStatus status);
	void contactActivityChanged(const Contact &contact, ChatStateService::State state);

public slots:
	void setChatMessagesRenderer(std::shared_ptr<ChatMessagesRenderer> chatMessagesRenderer);

private:
	Chat m_chat;
	QVector<Message> m_messages;
	Message m_lastMessage;
	std::shared_ptr<ChatMessagesRenderer> m_chatMessagesRenderer;

	bool m_pruneEnabled;
	bool m_forcePruneDisabled;

	void pruneMessages();

};
