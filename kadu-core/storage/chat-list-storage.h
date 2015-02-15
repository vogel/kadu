/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtCore/QString>

class Chat;
class ChatManager;
class StoragePoint;

class ChatListStorage
{

public:
	ChatListStorage(StoragePoint *storagePoint, QString nodeName);

	void setChatManager(ChatManager *chatManager);

	QVector<Chat> load() const;
	void store(const QVector<Chat> &chats) const;

private:
	QPointer<ChatManager> m_chatManager;
	StoragePoint *m_storagePoint;
	QString m_nodeName;

	QVector<Chat> chatsFromUuids(const QStringList &uuids) const;
	QStringList uuidsFromChats(const QVector<Chat> &chats) const;

};
