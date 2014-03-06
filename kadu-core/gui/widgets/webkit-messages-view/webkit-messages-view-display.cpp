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

#include "webkit-messages-view-display.h"

#include "chat-style/engine/chat-style-renderer.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"

WebkitMessagesViewDisplay::WebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer) :
		m_chatStyleRenderer(chatStyleRenderer)
{
}

WebkitMessagesViewDisplay::~WebkitMessagesViewDisplay()
{
}

void WebkitMessagesViewDisplay::setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory)
{
	m_messageRenderInfoFactory = messageRenderInfoFactory;
}

ChatStyleRenderer & WebkitMessagesViewDisplay::chatStyleRenderer() const
{
	return m_chatStyleRenderer;
}

void WebkitMessagesViewDisplay::appendMessagesRange(I from, I to, Message previousMessage, MessageRenderHeaderBehavior headerBehavior) const
{
	for (auto it = from; it != to; ++it)
	{
		auto info = m_messageRenderInfoFactory->messageRenderInfo(previousMessage, *it, headerBehavior);
		chatStyleRenderer().appendChatMessage(*it, info);
		previousMessage = *it;
	}
}
