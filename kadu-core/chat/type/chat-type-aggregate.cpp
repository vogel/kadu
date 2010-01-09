/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
	return IconsManager::instance()->loadIcon("OpenChat");
}

ChatDetails * ChatTypeAggregate::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsAggregate(chatData);
}
