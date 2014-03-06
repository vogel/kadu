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

#include "webkit-messages-view-removing-display.h"

#include "chat-style/engine/chat-style-renderer.h"
#include "message/message-render-header-behavior.h"
#include "misc/algorithm.h"

WebkitMessagesViewRemovingDisplay::WebkitMessagesViewRemovingDisplay(ChatStyleRenderer &chatStyleRenderer) :
		WebkitMessagesViewDisplay{chatStyleRenderer}
{
}

WebkitMessagesViewRemovingDisplay::~WebkitMessagesViewRemovingDisplay()
{
}

void WebkitMessagesViewRemovingDisplay::displayMessages(SortedMessages messages)
{
	auto overlapping = find_overlapping_region(begin(m_currentMessages), end(m_currentMessages), begin(messages), end(messages));
	auto previousMessage = Message::null;

	if (!m_currentMessages.empty())
	{
		if (end(m_currentMessages) == overlapping.first)
			chatStyleRenderer().clearMessages();
		else if (begin(m_currentMessages) != overlapping.first)
		{
			auto toRemove = std::distance(begin(m_currentMessages), overlapping.first);
			for (auto i = 0; i < toRemove; i++)
				chatStyleRenderer().removeFirstMessage();
			previousMessage = m_currentMessages.last();
		}
	}

	appendMessagesRange(overlapping.second, end(messages), previousMessage, MessageRenderHeaderBehavior::Always);
	m_currentMessages = std::move(messages);
}
