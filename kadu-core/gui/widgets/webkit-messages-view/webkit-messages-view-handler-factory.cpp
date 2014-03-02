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

#include "webkit-messages-view-handler-factory.h"

#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-renderer.h"
#include "gui/widgets/webkit-messages-view/message-limit-policy.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler.h"
#include "misc/kadu-paths.h"

WebkitMessagesViewHandlerFactory::WebkitMessagesViewHandlerFactory(QObject *parent) :
		QObject{parent}
{
}

WebkitMessagesViewHandlerFactory::~WebkitMessagesViewHandlerFactory()
{
}

void WebkitMessagesViewHandlerFactory::setChatStyleManager(ChatStyleManager *chatStyleManager)
{
	m_chatStyleManager = chatStyleManager;
}

void WebkitMessagesViewHandlerFactory::setWebkitMessagesViewDisplayFactory(WebkitMessagesViewDisplayFactory *webkitMessagesViewDisplayFactory)
{
	m_webkitMessagesViewDisplayFactory = webkitMessagesViewDisplayFactory;
}

qobject_ptr<WebkitMessagesViewHandler> WebkitMessagesViewHandlerFactory::createWebkitMessagesViewHandler(qobject_ptr<ChatStyleRenderer> chatStyleRenderer, QObject *parent)
{
	auto display = m_webkitMessagesViewDisplayFactory->createWebkitMessagesViewDisplay(*chatStyleRenderer.get());

	auto result = make_qobject<WebkitMessagesViewHandler>(std::move(chatStyleRenderer), std::move(display), parent);
	result->setMessageLimit(m_chatStyleManager->prune());
	result->setMessageLimitPolicy(0 == m_chatStyleManager->prune()
			? MessageLimitPolicy::None
			: MessageLimitPolicy::Value);

	return std::move(result);
}
