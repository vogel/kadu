/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-list-mime-data-service.h"

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "misc/memory.h"

#include <QtCore/QMimeData>

ChatListMimeDataService::ChatListMimeDataService(QObject *parent) :
		QObject{parent},
		m_mimeType{"application/x-kadu-chat-list"}
{
}

ChatListMimeDataService::~ChatListMimeDataService()
{
}

void ChatListMimeDataService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

QStringList ChatListMimeDataService::mimeTypes()
{
	return QStringList{} << m_mimeType;
}

std::unique_ptr<QMimeData> ChatListMimeDataService::toMimeData(const QList<Chat> &chatList)
{
	if (chatList.isEmpty())
		return 0;

	auto mimeData = std::make_unique<QMimeData>();
	auto chatListStrings = QStringList{};
	for (auto const &chat : chatList)
		chatListStrings << chat.uuid().toString();

	mimeData->setData(m_mimeType, chatListStrings.join(":").toUtf8());
	return mimeData;
}

QList<Chat> ChatListMimeDataService::fromMimeData(const QMimeData *mimeData)
{
	auto result = QList<Chat>{};
	auto chatListString = QString{mimeData->data(m_mimeType)};
	if (chatListString.isEmpty())
		return result;

	auto chatListStrings = chatListString.split(':');
	for (auto const &chatListString : chatListStrings)
	{
		auto chat = m_chatManager->byUuid(chatListString);
		if (!chat.isNull())
			result << chat;
	}

	return result;
}

#include "moc_chat-list-mime-data-service.cpp"
