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
#include "core/core.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/algorithm.h"

WebkitMessagesViewDisplay::WebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer) :
		m_chatStyleRenderer(chatStyleRenderer)
{
}

void WebkitMessagesViewDisplay::displayMessages(SortedMessages messages)
{
	auto difference = sequence_difference(begin(m_currentMessages), end(m_currentMessages), begin(messages), end(messages));
	auto lastMessage = Message::null;

	if (end(m_currentMessages) != difference.first)
	{
		auto toRemove = std::distance(end(m_currentMessages), difference.first);
		for (auto i = 0; i < toRemove; i++)
			m_chatStyleRenderer.removeFirstMessage();
		lastMessage = m_currentMessages.last();
	}
	else
		m_chatStyleRenderer.clearMessages();

	for (auto it = difference.second; it != end(messages); ++it)
	{
		auto info = Core::instance()->messageRenderInfoFactory()->messageRenderInfo(lastMessage, *it);
		m_chatStyleRenderer.appendChatMessage(*it, info);
		lastMessage = *it;
	}

	m_currentMessages = std::move(messages);
}
