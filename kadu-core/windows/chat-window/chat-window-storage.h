/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "windows/chat-window/chat-window-storage-configuration.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>
#include <memory>

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
    Q_INVOKABLE explicit ChatWindowStorage(QObject *parent = nullptr);
    virtual ~ChatWindowStorage();

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

private slots:
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setStoragePointFactory(StoragePointFactory *storagePointFactory);
};

/**
 * @}
 */
