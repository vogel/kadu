/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "chat-details.h"

ChatDetails::ChatDetails(Chat *chat) :
		StorableObject(chat->storage()), CurrentChat(chat)
{
}

ChatDetails::~ChatDetails()
{
}
