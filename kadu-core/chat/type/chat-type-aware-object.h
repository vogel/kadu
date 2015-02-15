/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeAwareObject
 * @author Rafal 'Vogel' Malinowski
 * @short Class that gets informed about chat type adding/removal from ChatTypeManager.
 *
 * Each class that inherits from ChatTypeAwareObject has to implement two methods:
 * chatTypeRegistered and chatTypeUnregistered that are called automatically when new ChatType
 * is added to ChatTypeManager or removed from it.
 */
class KADUAPI ChatTypeAwareObject : public AwareObject<ChatTypeAwareObject>
{

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method is called after new ChatType is added to ChatTypeManager.
	 * @param chatType just added ChatType object
	 *
	 * Method is called after new ChatType is added to ChatTypeManager.
	 * This method must be overridden.
	 */
	virtual void chatTypeRegistered(ChatType *chatType) = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method is called after ChatType is removed from ChatTypeManager.
	 * @param chatType removed ChatType object
	 *
	 * Method is called after ChatType is removed from ChatTypeManager.
	 * This method must be overridden.
	 */
	virtual void chatTypeUnregistered(ChatType *chatType) = 0;

public:
	static void notifyChatTypeRegistered(ChatType *chatType);
	static void notifyChatTypeUnregistered(ChatType *chatType);

	void triggerAllChatTypesRegistered();
	void triggerAllChatTypesUnregistered();

};

/**
 * @}
 */

#endif // CHAT_TYPE_AWARE_OBJECT
