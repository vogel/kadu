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

#include "kadu-style-renderer-factory.h"

#include "chat-style/engine/kadu/kadu-style-renderer.h"
#include "core/injected-factory.h"

KaduStyleRendererFactory::KaduStyleRendererFactory(std::shared_ptr<KaduChatSyntax> style, QObject *parent) :
		QObject{parent},
		m_style{std::move(style)}
{
}

KaduStyleRendererFactory::~KaduStyleRendererFactory()
{
}

void KaduStyleRendererFactory::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

not_owned_qptr<ChatStyleRenderer> KaduStyleRendererFactory::createChatStyleRenderer(ChatStyleRendererConfiguration configuration)
{
	auto renderer = m_injectedFactory->makeNotOwned<KaduStyleRenderer>(std::move(configuration), m_style);
	return not_owned_qptr<ChatStyleRenderer>{renderer.release()};
}

#include "moc_kadu-style-renderer-factory.cpp"
