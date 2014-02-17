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

#include "chat/style-engine/chat-messages-renderer.h"

#include "chat/style-engine/adium-style-engine/adium-style.h"

class MessageHtmlRendererService;

class AdiumChatMessagesRenderer : public QObject, public ChatMessagesRenderer
{
	Q_OBJECT

public:
	explicit AdiumChatMessagesRenderer(AdiumStyle style);
	virtual ~AdiumChatMessagesRenderer() {}

	void setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService);

	virtual void clearMessages(QWebFrame &frame) override;
	virtual void appendChatMessage(QWebFrame &frame, const Message &newMessage, const Message &lastMessage) override;
	virtual void paintMessages(QWebFrame &frame, const Chat &chat, const QVector<Message> &messages) override;
	virtual void removeFirstMessage(QWebFrame &frame) override;
	virtual void refreshView(QWebFrame &frame, const Chat &chat, const QVector<Message> &allMessages, bool useTransparency = false) override;
	virtual void messageStatusChanged(QWebFrame &frame, Message, MessageStatus) override;
	virtual void contactActivityChanged(QWebFrame &frame, ChatStateService::State, const QString &, const QString &) override;
	virtual void chatImageAvailable(QWebFrame &frame, const ChatImage &chatImage, const QString &fileName) override;

private:
	QPointer<MessageHtmlRendererService> m_messageHtmlRendererService;

	AdiumStyle m_style;
	QString m_jsCode;

	QString replaceKeywords(const Chat &chat, const QString &styleHref, const QString &style);
	QString replaceKeywords(const QString &styleHref, const QString &source, const Message &message, const QString &nickColor);
	QString preprocessStyleBaseHtml(AdiumStyle &style, const Chat &chat);

};
