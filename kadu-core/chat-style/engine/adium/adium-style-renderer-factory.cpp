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

#include "adium-style-renderer-factory.h"

#include "chat-style/engine/adium/adium-style-renderer.h"
#include "message/message-html-renderer-service.h"

AdiumStyleRendererFactory::AdiumStyleRendererFactory(std::shared_ptr<AdiumStyle> style) :
		m_style{std::move(style)}
{
}

AdiumStyleRendererFactory::~AdiumStyleRendererFactory()
{
}

void AdiumStyleRendererFactory::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	m_messageHtmlRendererService = messageHtmlRendererService;
}

qobject_ptr<ChatStyleRenderer> AdiumStyleRendererFactory::createChatStyleRenderer(ChatStyleRendererConfiguration configuration)
{
	auto renderer = make_qobject<AdiumStyleRenderer>(std::move(configuration), m_style);
	renderer->setMessageHtmlRendererService(m_messageHtmlRendererService);
	return qobject_ptr<ChatStyleRenderer>{renderer.release()};
}