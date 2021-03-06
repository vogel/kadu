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

#include "configured-chat-style-renderer-factory-provider.h"
#include "configured-chat-style-renderer-factory-provider.moc"

#include "chat-style/engine/chat-style-renderer-factory.h"

ConfiguredChatStyleRendererFactoryProvider::ConfiguredChatStyleRendererFactoryProvider(QObject *parent)
        : ChatStyleRendererFactoryProvider{parent}
{
}

ConfiguredChatStyleRendererFactoryProvider::~ConfiguredChatStyleRendererFactoryProvider()
{
}

std::shared_ptr<ChatStyleRendererFactory> ConfiguredChatStyleRendererFactoryProvider::chatStyleRendererFactory() const
{
    return m_chatStyleRendererFactory;
}

void ConfiguredChatStyleRendererFactoryProvider::setChatStyleRendererFactory(
    std::unique_ptr<ChatStyleRendererFactory> chatStyleRendererFactory)
{
    m_chatStyleRendererFactory = std::move(chatStyleRendererFactory);
    emit chatStyleRendererFactoryChanged(m_chatStyleRendererFactory);
}
