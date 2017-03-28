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

#include "chat-state-service-repository.h"
#include "chat-state-service-repository.moc"

#include "protocols/services/chat-state-service.h"

#include <cassert>

namespace
{
ChatStateService *converter(ChatStateServiceRepository::WrappedIterator iterator)
{
    return iterator->second;
}
}

ChatStateServiceRepository::Iterator ChatStateServiceRepository::begin()
{
    return Iterator{m_chatStateServices.begin(), converter};
}

ChatStateServiceRepository::Iterator ChatStateServiceRepository::end()
{
    return Iterator{m_chatStateServices.end(), converter};
}

ChatStateService *ChatStateServiceRepository::chatStateService(const Account &account) const
{
    auto it = m_chatStateServices.find(account);
    return it == std::end(m_chatStateServices) ? nullptr : it->second;
}

void ChatStateServiceRepository::addChatStateService(ChatStateService *chatStateService)
{
    assert(m_chatStateServices.find(chatStateService->account()) == std::end(m_chatStateServices));

    m_chatStateServices.insert(std::make_pair(chatStateService->account(), chatStateService));
    emit chatStateServiceAdded(chatStateService);
}

void ChatStateServiceRepository::removeChatStateService(ChatStateService *chatStateService)
{
    auto it = m_chatStateServices.find(chatStateService->account());
    assert(it != std::end(m_chatStateServices));

    m_chatStateServices.erase(it);
    emit chatStateServiceRemoved(chatStateService);
}
