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

#include <QtCore/QPointer>

#include "message/sorted-messages.h"

class ChatStyleRenderer;
class MessageRenderInfoFactory;

enum class MessageRenderHeaderBehavior;

class WebkitMessagesViewDisplay
{

public:
	explicit WebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer);
	virtual ~WebkitMessagesViewDisplay();

	void setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory);

	virtual void displayMessages(SortedMessages messages) = 0;

protected:
	ChatStyleRenderer & chatStyleRenderer() const;

	using I = decltype(begin(std::declval<SortedMessages>()));
	void displayMessagesRange(I from, I to, Message previousMessage, MessageRenderHeaderBehavior headerBehavior) const;

private:
	QPointer<MessageRenderInfoFactory> m_messageRenderInfoFactory;

	ChatStyleRenderer &m_chatStyleRenderer;

};
