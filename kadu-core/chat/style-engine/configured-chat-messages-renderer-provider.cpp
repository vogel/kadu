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

#include "configured-chat-messages-renderer-provider.h"

#include "chat/style-engine/chat-messages-renderer.h"

ConfiguredChatMessagesRendererProvider::ConfiguredChatMessagesRendererProvider(QObject *parent) :
		ChatMessagesRendererProvider{parent}
{
}

ConfiguredChatMessagesRendererProvider::~ConfiguredChatMessagesRendererProvider()
{
}

std::shared_ptr<ChatMessagesRenderer> ConfiguredChatMessagesRendererProvider::chatMessagesRenderer() const
{
	return m_chatMessagesRenderer;
}

void ConfiguredChatMessagesRendererProvider::setChatMessagesRenderer(std::unique_ptr<ChatMessagesRenderer> chatMessagesRenderer)
{
	m_chatMessagesRenderer = std::move(chatMessagesRenderer);
	emit chatMessagesRendererChanged(m_chatMessagesRenderer);
}

#include "moc_configured-chat-messages-renderer-provider.cpp"
