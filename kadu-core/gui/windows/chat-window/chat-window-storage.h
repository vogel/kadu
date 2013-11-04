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

#pragma once

#include "storage/storable-string-list.h"
#include "gui/windows/chat-window/chat-window-storage-configuration.h"

#include <QtCore/QVector>

class Chat;
class ChatManager;
class ChatWindowRepository;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWindowStorage
 * @short Class for storing and loading list of active ChatWindows.
 *
 * This class stores and loads lists of chats that have opened ChatWindow instances from
 * persistent storage. It uses ChatWindowStorageConfiguration to check if list
 * should be stored or not.
 *
 * List of chats to store is fetched from @see ChatWindowRepository object.
 * Mapping from stored ids to chats is done using @see ChatManager object.
 */
class ChatWindowStorage : public QObject, public StorableStringList
{
	Q_OBJECT

public:
	explicit ChatWindowStorage(QObject *parent = nullptr);
	virtual ~ChatWindowStorage();

	void setChatManager(ChatManager *chatManager);
	void setChatWindowRepository(ChatWindowRepository *chatWindowRepository);

	void setConfiguration(ChatWindowStorageConfiguration configuration);

	/**
	 * @short Return list of chats loaded from storage.
	 */
	QVector<Chat> loadedChats();

	virtual StorableObject * storageParent() override;
	virtual QString storageNodeName() override;
	virtual QString storageItemNodeName() override;

protected:
	virtual void load() override;
	virtual void store() override;

private:
	QWeakPointer<ChatManager> m_chatManager;
	QWeakPointer<ChatWindowRepository> m_chatWindowRepository;
	ChatWindowStorageConfiguration m_configuration;
	QVector<Chat> m_loadedChats;

};

/**
 * @}
 */
