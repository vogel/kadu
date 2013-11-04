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

#include "chat-window-storage.h"

#include "chat/chat-manager.h"
#include "storage/storage-point-factory.h"
#include "storage/string-list-storage.h"

ChatWindowStorage::ChatWindowStorage(QObject *parent) :
		QObject(parent)
{
}

ChatWindowStorage::~ChatWindowStorage()
{
}

void ChatWindowStorage::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void ChatWindowStorage::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

void ChatWindowStorage::setConfiguration(ChatWindowStorageConfiguration configuration)
{
	m_configuration = configuration;
}

std::unique_ptr<StoragePoint> ChatWindowStorage::storagePoint() const
{
	if (!m_storagePointFactory)
		return {};
	return m_storagePointFactory.data()->createStoragePoint(QLatin1String("ChatWindows"));
}

QVector<Chat> ChatWindowStorage::loadChats()
{
	auto storage = storagePoint();
	if (!storage)
		return {};

	auto stringListStorage = StringListStorage(storage.get(), QLatin1String("Chat"));
	return chatsFromUuids(stringListStorage.load());
}

QVector<Chat> ChatWindowStorage::chatsFromUuids(const QStringList &uuids) const
{
	if (!m_chatManager)
		return {};

	auto result = QVector<Chat>();
	std::transform(uuids.begin(), uuids.end(), std::back_inserter(result), [this](const QString &uuid){
		return m_chatManager.data()->byUuid(uuid);
	});
	return result;
}

void ChatWindowStorage::storeChats(const QVector<Chat> &chats)
{
	auto storage = storagePoint();
	if (!storage)
		return;

	auto stringListStorage = StringListStorage(storage.get(), QLatin1String("Chat"));
	stringListStorage.store(uuidsFromChats(chats));
}

QStringList ChatWindowStorage::uuidsFromChats(const QVector<Chat> &chats)
{
	auto result = QStringList();
	std::transform(chats.begin(), chats.end(), std::back_inserter(result), [this](const Chat &chat){
		return chat.uuid().toString();
	});
	return result;
}

#include "moc_chat-window-storage.cpp"
