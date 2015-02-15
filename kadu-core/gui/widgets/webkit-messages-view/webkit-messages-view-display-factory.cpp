/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "webkit-messages-view-display-factory.h"

#include "chat-style/chat-style-manager.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-clearing-display.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-removing-display.h"
#include "message/message-render-info-factory.h"
#include "misc/memory.h"

WebkitMessagesViewDisplayFactory::WebkitMessagesViewDisplayFactory(QObject *parent):
		QObject{parent}
{
}

WebkitMessagesViewDisplayFactory::~WebkitMessagesViewDisplayFactory()
{
}

void WebkitMessagesViewDisplayFactory::setChatStyleManager(ChatStyleManager *chatStyleManager)
{
	m_chatStyleManager = chatStyleManager;
}

void WebkitMessagesViewDisplayFactory::setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory)
{
	m_messageRenderInfoFactory = messageRenderInfoFactory;
}

std::unique_ptr<WebkitMessagesViewDisplay> WebkitMessagesViewDisplayFactory::createWebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer)
{
	if (m_chatStyleManager && m_chatStyleManager->cfgNoHeaderRepeat())
	{
		auto result = make_unique<WebkitMessagesViewClearingDisplay>(chatStyleRenderer);
		result->setMessageRenderInfoFactory(m_messageRenderInfoFactory);
		return std::move(result);
	}
	else
	{
		auto result = make_unique<WebkitMessagesViewRemovingDisplay>(chatStyleRenderer);
		result->setMessageRenderInfoFactory(m_messageRenderInfoFactory);
		return std::move(result);
	}

}
