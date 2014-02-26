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

#include "message/sorted-messages.h"

#include <QtCore/QPointer>

class ChatStyleRenderer;
class MessageRenderInfoFactory;

class WebkitMessagesViewDisplay
{

public:
	explicit WebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer);

	void setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory);

	void displayMessages(SortedMessages messages);

private:
	QPointer<MessageRenderInfoFactory> m_messageRenderInfoFactory;

	ChatStyleRenderer &m_chatStyleRenderer;
	SortedMessages m_currentMessages;

};
