/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef AGGREGATE_CHAT_BUILDER_H
#define AGGREGATE_CHAT_BUILDER_H

#include <QtCore/QObject>

#include "exports.h"

class BuddySet;
class Chat;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class AggregateChatBuilder
 * @short Makes chat object that aggregates all chats for given buddy set.
 *
 * Class contains one static method that makes chat object that
 * aggregates all chats for given buddy set.
 */
class KADUAPI AggregateChatBuilder
{
public:
	static Chat buildAggregateChat(BuddySet buddies);

};

/**
 * @}
 */

#endif // AGGREGATE_CHAT_BUILDER_H
