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

#include "recent-chat-service.h"

#include "chat/chat-manager.h"
#include "chat/recent-chat-repository.h"
#include "message/message-manager.h"

const QString RecentChatService::LAST_MESSAGE_DATE_TIME_PROPERTY{"recent:lastMessageDateTime"};

RecentChatService::RecentChatService(QObject *parent) : QObject{parent}
{
}

RecentChatService::~RecentChatService()
{
}

void RecentChatService::cleanUp()
{
    auto oldChats = std::vector<Chat>{};
    for (auto chat : m_recentChatRepository)
        if (!isRecent(chat))
            oldChats.push_back(chat);
    for (auto chat : oldChats)
        remove(chat);
}

void RecentChatService::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void RecentChatService::setMessageManager(MessageManager *messageManager)
{
    m_messageManager = messageManager;
}

void RecentChatService::setOpenChatRepository(RecentChatRepository *recentChatRepository)
{
    m_recentChatRepository = recentChatRepository;
}

void RecentChatService::init()
{
    connect(m_chatManager, &ChatManager::chatAdded, this, &RecentChatService::chatAdded);
    connect(m_chatManager, &ChatManager::chatRemoved, this, &RecentChatService::chatRemoved);
    connect(m_messageManager, &MessageManager::messageReceived, this, &RecentChatService::message);
    connect(m_messageManager, &MessageManager::messageSent, this, &RecentChatService::message);

    for (auto const &chat : m_chatManager->items())
        update(chat);
}

void RecentChatService::update(Chat chat) const
{
    if (isRecent(chat) && !isAlreadyInRepository(chat))
        m_recentChatRepository->addRecentChat(chat);
    if (!isRecent(chat))
        remove(chat);
}

void RecentChatService::add(Chat chat) const
{
    chat.addProperty(LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime::currentDateTimeUtc(), CustomProperties::Storable);
    update(chat);
}

void RecentChatService::remove(Chat chat) const
{
    chat.removeProperty(LAST_MESSAGE_DATE_TIME_PROPERTY);
    m_recentChatRepository->removeRecentChat(chat);
}

bool RecentChatService::isRecent(Chat chat) const
{
    if (!chat.hasProperty(LAST_MESSAGE_DATE_TIME_PROPERTY))
        return false;

    auto dateTime = chat.property(LAST_MESSAGE_DATE_TIME_PROPERTY, QDateTime{}).toDateTime();
    return isRecent(dateTime);
}

bool RecentChatService::isRecent(QDateTime dateTime) const
{
    return dateTime.addSecs(KEEP_RECENT_FOR_SECONDS) >= QDateTime::currentDateTimeUtc();
}

bool RecentChatService::isAlreadyInRepository(Chat chat) const
{
    return std::find(begin(m_recentChatRepository), end(m_recentChatRepository), chat) != end(m_recentChatRepository);
}

void RecentChatService::message(const Message &message) const
{
    add(message.messageChat());
}

void RecentChatService::chatAdded(Chat chat) const
{
    update(chat);
}

void RecentChatService::chatRemoved(Chat chat) const
{
    remove(chat);
}

#include "recent-chat-service.moc"
