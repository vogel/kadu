/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QVariant>
#include <QtGui/QIcon>

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "talkable/talkable.h"

#include "chat-data-extractor.h"

QVariant ChatDataExtractor::data(const Chat &chat, int role)
{
	if (chat.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			if (!chat.display().isEmpty())
				return chat.display();
			if (!chat.name().isEmpty())
				return chat.name();
			/// @todo this is a hack, remove it by creating HistoryTalkableProxyModel with different ::data() method
			return chat.property("sql_history:id", chat.uuid().toString());
		}
		case Qt::DecorationRole:
		{
			QString chatTypeName = chat.type();
			ChatType *chatType = ChatTypeManager::instance()->chatType(chatTypeName);
			if (chatType)
				return chatType->icon().icon();
			else
				return KaduIcon("internet-group-chat").icon();
		}
		case AccountRole:
			return QVariant::fromValue(chat.chatAccount());
		case ChatRole:
			return QVariant::fromValue(chat);
		case ItemTypeRole:
			return ChatRole;
		case TalkableRole:
			return QVariant::fromValue(Talkable(chat));
		default:
			return QVariant();
	}
}
