/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "chat/chat-details-aggregate.h"

#include "icons-manager.h"

#include "chat-type-aggregate.h"

int ChatTypeAggregate::sortIndex() const
{
	return -1;
}

QString ChatTypeAggregate::name() const
{
	return "Aggregate";
}

QString ChatTypeAggregate::displayName() const
{
	return tr("Aggregate");
}

QIcon ChatTypeAggregate::icon() const
{
	return IconsManager::instance()->iconByPath("kadu/kadu-chat.png");
}

ChatDetails * ChatTypeAggregate::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsAggregate(chatData);
}
