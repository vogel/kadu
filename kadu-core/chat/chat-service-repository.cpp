/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-service-repository.h"
#include "chat-service-repository.moc"

#include "accounts/account.h"

#include <cassert>

ChatService * ChatServiceRepository::chatService(const Account &account) const
{
	auto it = m_chatServices.find(account);
	return it == std::end(m_chatServices)
			? nullptr
			: it->second;
}

void ChatServiceRepository::addChatService(const Account &account, ChatService *chatService)
{
	assert(m_chatServices.find(account) == std::end(m_chatServices));

	m_chatServices.insert(std::make_pair(account, chatService));
	emit chatServiceAdded(account, chatService);
}

void ChatServiceRepository::removeChatService(const Account &account)
{
	auto it = m_chatServices.find(account);
	assert(it != std::end(m_chatServices));

	auto chatService = it->second;
	m_chatServices.erase(it);
	emit chatServiceRemoved(account, chatService);
}
