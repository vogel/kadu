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

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "gui/windows/chat-window/chat-window.h"
#include "gui/windows/chat-window/chat-window-repository.h"
#include "protocols/protocol.h"

ChatWindowStorage::ChatWindowStorage(QObject *parent) :
		QObject(parent)
{
	setState(StateNotLoaded);
}

ChatWindowStorage::~ChatWindowStorage()
{
}

void ChatWindowStorage::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void ChatWindowStorage::setChatWindowRepository(ChatWindowRepository *chatWindowRepository)
{
	m_chatWindowRepository = chatWindowRepository;
}

void ChatWindowStorage::setConfiguration(ChatWindowStorageConfiguration configuration)
{
	m_configuration = configuration;
}

QVector<Chat> ChatWindowStorage::loadedChats()
{
	ensureLoaded();

	return m_loadedChats;
}

StorableObject * ChatWindowStorage::storageParent()
{
	return nullptr;
}

QString ChatWindowStorage::storageNodeName()
{
	return QLatin1String("ChatWindows");
}

QString ChatWindowStorage::storageItemNodeName()
{
	return QLatin1String("Chat");
}

void ChatWindowStorage::load()
{
	if (!isValidStorage())
		return;

	m_loadedChats.clear();
	StorableStringList::load();

	if (m_chatManager && m_configuration.storeOpenedChatWindows())
	{
		foreach (const auto &uuid, content())
		{
			auto chatUuid = QUuid(uuid);
			auto chat = m_chatManager.data()->byUuid(chatUuid);
			if (chat)
				m_loadedChats.append(chat);
		}
	}
}

void ChatWindowStorage::store()
{
	if (!isValidStorage())
		return;

	StringList.clear();

	if (m_chatWindowRepository && m_configuration.storeOpenedChatWindows())
	{
		foreach (const auto &window, m_chatWindowRepository.data()->windows())
		{
			auto chat = window->chat();
			auto protocolHandler = chat.chatAccount().protocolHandler();
			if (protocolHandler && protocolHandler->protocolFactory())
				StringList.append(chat.uuid().toString());
		}
	}

	StorableStringList::store();
}

#include "moc_chat-window-storage.cpp"
