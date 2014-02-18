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

#include "adium-chat-messages-renderer-factory.h"

#include "chat/style-engine/adium-style-engine/adium-chat-messages-renderer.h"
#include "message/message-html-renderer-service.h"
#include "misc/memory.h"

AdiumChatMessagesRendererFactory::AdiumChatMessagesRendererFactory(std::shared_ptr<AdiumStyle> style, QString jsCode) :
		m_style{std::move(style)},
		m_jsCode{jsCode}
{
}

AdiumChatMessagesRendererFactory::~AdiumChatMessagesRendererFactory()
{
}

void AdiumChatMessagesRendererFactory::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	m_messageHtmlRendererService = messageHtmlRendererService;
}

std::unique_ptr<ChatMessagesRenderer> AdiumChatMessagesRendererFactory::createChatMessagesRenderer(Chat chat, QWebFrame &frame)
{
	auto result = make_unique<AdiumChatMessagesRenderer>(chat, frame, m_style, m_jsCode);
	result->setMessageHtmlRendererService(m_messageHtmlRendererService);
	return std::move(result);
}
