/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-window-factory.h"

#include "gui/windows/chat-window/chat-window.h"
#include "message/unread-message-repository.h"

ChatWindowFactory::ChatWindowFactory(QObject *parent) :
		QObject(parent)
{
}

ChatWindowFactory::~ChatWindowFactory()
{
}

void ChatWindowFactory::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

std::unique_ptr<ChatWindow> ChatWindowFactory::createChatWindow(ChatWidget *chatWidget)
{
	auto result = std::unique_ptr<ChatWindow>(new ChatWindow(chatWidget));
	result.get()->setUnreadMessageRepository(m_unreadMessageRepository.data());

	return result;
}
