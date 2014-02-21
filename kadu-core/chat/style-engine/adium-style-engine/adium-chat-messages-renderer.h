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
class QWebFrame;

class AdiumChatMessagesRenderer : public ChatMessagesRenderer
{
	Q_OBJECT

public:
	explicit AdiumChatMessagesRenderer(ChatMessagesRendererConfiguration configuration, std::shared_ptr<AdiumStyle> style, QObject *parent = nullptr);
	virtual ~AdiumChatMessagesRenderer();

	void setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService);

	virtual void initialize();
	virtual void clearMessages() override;
	virtual void appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo) override;
	virtual void paintMessages(const QVector<Message> &messages) override;
	virtual void removeFirstMessage() override;
	virtual void refreshView(const QVector<Message> &allMessages) override;
	virtual void messageStatusChanged(Message, MessageStatus) override;
	virtual void contactActivityChanged(ChatStateService::State, const QString &, const QString &) override;
	virtual void chatImageAvailable(const ChatImage &chatImage, const QString &fileName) override;

private:
	QPointer<MessageHtmlRendererService> m_messageHtmlRendererService;

	std::shared_ptr<AdiumStyle> m_style;

	QString replaceKeywords(const QString &styleHref, const QString &style);
	QString replaceKeywords(const QString &styleHref, const QString &source, const Message &message, const QString &nickColor);
	QString preprocessStyleBaseHtml(bool useTransparency);

};
