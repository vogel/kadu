/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-module.h"

#include "chat/buddy-chat-manager.h"
#include "chat/chat-list-mime-data-service.h"
#include "chat/chat-manager-impl.h"
#include "chat/chat-service-repository.h"
#include "chat/chat-storage.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/open-chat-repository.h"
#include "chat/open-chat-service.h"
#include "chat/recent-chat-repository.h"
#include "chat/recent-chat-service.h"
#include "chat/type/chat-type-buddy.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-manager.h"
#include "chat/type/chat-type-room.h"

ChatModule::ChatModule()
{
	add_type<BuddyChatManager>();
	add_type<ChatDataExtractor>();
	add_type<ChatListMimeDataService>();
	add_type<ChatManagerImpl>();
	add_type<ChatServiceRepository>();
	add_type<ChatStorage>();
	add_type<ChatTypeBuddy>();
	add_type<ChatTypeContactSet>();
	add_type<ChatTypeContact>();
	add_type<ChatTypeManager>();
	add_type<ChatTypeRoom>();
	add_type<OpenChatRepository>();
	add_type<OpenChatService>();
	add_type<RecentChatRepository>();
	add_type<RecentChatService>();
}

ChatModule::~ChatModule()
{
}
