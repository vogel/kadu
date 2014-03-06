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

#include "webkit-messages-view-clearing-display.h"

#include "chat-style/engine/chat-style-renderer.h"
#include "message/message-render-header-behavior.h"
#include "misc/algorithm.h"

WebkitMessagesViewClearingDisplay::WebkitMessagesViewClearingDisplay(ChatStyleRenderer &chatStyleRenderer) :
		WebkitMessagesViewDisplay{chatStyleRenderer}
{
}

WebkitMessagesViewClearingDisplay::~WebkitMessagesViewClearingDisplay()
{
}

void WebkitMessagesViewClearingDisplay::displayMessages(SortedMessages messages)
{
	auto overlapping = find_overlapping_region(begin(m_currentMessages), end(m_currentMessages), begin(messages), end(messages));

	if (!m_currentMessages.empty() && begin(m_currentMessages) != overlapping.first)
	{
		chatStyleRenderer().clearMessages();
		appendMessagesRange(begin(messages), end(messages), Message::null, MessageRenderHeaderBehavior::WhenRequired);
	}
	else
		appendMessagesRange(overlapping.second, end(messages), m_currentMessages.last(), MessageRenderHeaderBehavior::WhenRequired);

	m_currentMessages = std::move(messages);
}
