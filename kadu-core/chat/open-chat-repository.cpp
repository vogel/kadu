/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-chat-repository.h"

#include <algorithm>

OpenChatRepository::OpenChatRepository(QObject *parent) :
		QObject{parent}
{
}

OpenChatRepository::~OpenChatRepository()
{
}

OpenChatRepository::Iterator OpenChatRepository::begin() const
{
	return m_chats.cbegin();
}

OpenChatRepository::Iterator OpenChatRepository::end() const
{
	return m_chats.cend();
}

void OpenChatRepository::addOpenChat(Chat chat)
{
	m_chats.push_back(chat);
	emit openChatAdded(chat);
}

void OpenChatRepository::removeOpenChat(Chat chat)
{
	m_chats.erase(std::remove(std::begin(m_chats), std::end(m_chats), chat), std::end(m_chats));
	emit openChatRemoved(chat);
}

size_t OpenChatRepository::size() const
{
	return m_chats.size();
}
