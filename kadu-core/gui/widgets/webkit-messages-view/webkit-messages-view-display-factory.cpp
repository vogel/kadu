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

#include "webkit-messages-view-display-factory.h"

#include "gui/widgets/webkit-messages-view/webkit-messages-view-display.h"
#include "message/message-render-info-factory.h"
#include "misc/memory.h"

WebkitMessagesViewDisplayFactory::WebkitMessagesViewDisplayFactory(QObject *parent):
		QObject{parent}
{
}

WebkitMessagesViewDisplayFactory::~WebkitMessagesViewDisplayFactory()
{
}

void WebkitMessagesViewDisplayFactory::setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory)
{
	m_messageRenderInfoFactory = messageRenderInfoFactory;
}

std::unique_ptr<WebkitMessagesViewDisplay> WebkitMessagesViewDisplayFactory::createWebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer)
{
	auto result = make_unique<WebkitMessagesViewDisplay>(chatStyleRenderer);
	result->setMessageRenderInfoFactory(m_messageRenderInfoFactory);

	return std::move(result);
}
