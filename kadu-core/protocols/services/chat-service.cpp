/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "services/message-filter-service.h"
#include "services/message-transformer-service.h"

#include "chat-service.h"

ChatService::ChatService(Account account, QObject *parent) :
		AccountService(account, parent)
{
}

ChatService::~ChatService()
{
}

void ChatService::setMessageFilterService(MessageFilterService *messageFilterService)
{
	CurrentMessageFilterService = messageFilterService;
}

MessageFilterService * ChatService::messageFilterService() const
{
	return CurrentMessageFilterService.data();
}

void ChatService::setMessageTransformerService(MessageTransformerService *messageTransformerService)
{
	CurrentMessageTransformerService = messageTransformerService;
}

MessageTransformerService * ChatService::messageTransformerService() const
{
	return CurrentMessageTransformerService.data();
}
