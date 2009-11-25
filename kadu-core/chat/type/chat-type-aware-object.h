/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
