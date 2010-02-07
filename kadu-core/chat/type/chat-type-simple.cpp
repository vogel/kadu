/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-details-simple.h"
#include "icons-manager.h"

#include "chat-type-simple.h"

int ChatTypeSimple::sortIndex() const
{
	return 0;
}

QString ChatTypeSimple::name() const
{
	return "Simple";
}

QString ChatTypeSimple::displayName() const
{
	return tr("Chat");
}

QIcon ChatTypeSimple::icon() const
{
	return IconsManager::instance()->iconByName("OpenChat");
}

ChatDetails * ChatTypeSimple::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsSimple(chatData);
}
