/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-list-storage.h"

#include "chat/chat-manager.h"
#include "storage/string-list-storage.h"

#include <QtCore/QVector>

ChatListStorage::ChatListStorage(StoragePoint *storagePoint, QString nodeName) :
		m_storagePoint(storagePoint), m_nodeName(nodeName)
{
}

void ChatListStorage::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

QVector<Chat> ChatListStorage::load() const
{
	auto stringListStorage = StringListStorage{m_storagePoint, m_nodeName};
	return chatsFromUuids(stringListStorage.load());
}

QVector<Chat> ChatListStorage::chatsFromUuids(const QStringList &uuids) const
{
	if (!m_chatManager)
		return {};

	auto result = QVector<Chat>{};
	std::transform(uuids.begin(), uuids.end(), std::back_inserter(result), [this](const QString &uuid){
		return m_chatManager.data()->byUuid(uuid);
	});
	return result;
}

void ChatListStorage::store(const QVector<Chat> &chats) const
{
	auto stringListStorage = StringListStorage{m_storagePoint, m_nodeName};
	stringListStorage.store(uuidsFromChats(chats));
}

QStringList ChatListStorage::uuidsFromChats(const QVector<Chat> &chats) const
{
	auto result = QStringList{};
	std::transform(chats.begin(), chats.end(), std::back_inserter(result), [this](const Chat &chat){
		return chat.uuid().toString();
	});
	return result;
}
