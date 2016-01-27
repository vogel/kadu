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

#include "adium-style-renderer-factory.h"

#include "chat-style/engine/adium/adium-style-renderer.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "message/message-html-renderer-service.h"

AdiumStyleRendererFactory::AdiumStyleRendererFactory(std::shared_ptr<AdiumStyle> style, QObject *parent) :
		QObject{parent},
		m_style{std::move(style)}
{
}

AdiumStyleRendererFactory::~AdiumStyleRendererFactory()
{
}

void AdiumStyleRendererFactory::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

not_owned_qptr<ChatStyleRenderer> AdiumStyleRendererFactory::createChatStyleRenderer(ChatStyleRendererConfiguration configuration)
{
	auto result = m_injectedFactory->makeNotOwned<AdiumStyleRenderer>(std::move(configuration), m_style);
	return not_owned_qptr<ChatStyleRenderer>(result.release());
}

#include "moc_adium-style-renderer-factory.cpp"
