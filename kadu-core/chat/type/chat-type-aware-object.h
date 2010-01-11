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

#ifndef CHAT_TYPE_AWARE_OBJECT
#define CHAT_TYPE_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class ChatType;

class KADUAPI ChatTypeAwareObject : public AwareObject<ChatTypeAwareObject>
{

protected:
	virtual void chatTypeRegistered(ChatType *chatType) = 0;
	virtual void chatTypeUnregistered(ChatType *chatType) = 0;

public:
	static void notifyChatTypeRegistered(ChatType *chatType);
	static void notifyChatTypeUnregistered(ChatType *chatType);

	void triggerAllChatTypesRegistered();
	void triggerAllChatTypesUnregistered();

};

#endif // CHAT_TYPE_AWARE_OBJECT
