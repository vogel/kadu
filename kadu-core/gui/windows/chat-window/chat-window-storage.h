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

#include "gui/windows/chat-window/chat-window-storage-configuration.h"

#include <memory>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QPointer>

class Chat;
class ChatManager;
class StoragePoint;
class StoragePointFactory;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWindowStorage
 * @short Class for storing and loading list of active ChatWindows.
 *
 * This class stores and loads lists of chats from ChatWindows configuration node.
 * It uses ChatWindowStorageConfiguration to check if list should be stored or not.
 */
class ChatWindowStorage : public QObject
{
	Q_OBJECT

public:
	explicit ChatWindowStorage(QObject *parent = nullptr);
	virtual ~ChatWindowStorage();

	void setChatManager(ChatManager *chatManager);
	void setStoragePointFactory(StoragePointFactory *storagePointFactory);

	void setConfiguration(ChatWindowStorageConfiguration configuration);

	/**
	 * @short Return list of chats loaded from persistent storage.
	 */
	QVector<Chat> loadChats() const;

	/**
	 * @short Store list of chats to persistent storage.
	 */
	void storeChats(const QVector<Chat> &chats);

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<StoragePointFactory> m_storagePointFactory;
	ChatWindowStorageConfiguration m_configuration;

	std::unique_ptr<StoragePoint> storagePoint() const;

};

/**
 * @}
 */
