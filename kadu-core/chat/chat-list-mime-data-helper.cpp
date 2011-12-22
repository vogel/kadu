/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMimeData>
#include <QtCore/QStringList>

#include "chat/chat-manager.h"
#include "chat/chat.h"

#include "chat-list-mime-data-helper.h"

QLatin1String ChatListMimeDataHelper::MimeType("application/x-kadu-chat-list");

QStringList ChatListMimeDataHelper::mimeTypes()
{
	QStringList result;
	result << MimeType;
	return result;
}

QMimeData * ChatListMimeDataHelper::toMimeData(const QList<Chat> &chatList)
{
	if (chatList.isEmpty())
		return 0;

	QMimeData *mimeData = new QMimeData();

	QStringList chatListStrings;
	foreach (const Chat &chat, chatList)
		chatListStrings << chat.uuid().toString();

	mimeData->setData(MimeType, chatListStrings.join(":").toUtf8());
	return mimeData;
}

QList<Chat> ChatListMimeDataHelper::fromMimeData(const QMimeData *mimeData)
{
	QList<Chat> result;

	QString chatListString(mimeData->data(MimeType));
	if (chatListString.isEmpty())
		return result;

	QStringList chatListStrings = chatListString.split(':');
	foreach (const QString &chatListString, chatListStrings)
	{
		Chat chat = ChatManager::instance()->byUuid(chatListString);
		if (chat.isNull())
			continue;

		result << chat;
	}

	return result;
}
