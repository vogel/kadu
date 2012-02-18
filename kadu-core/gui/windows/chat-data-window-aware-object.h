/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_DATA_WINDOW_AWARE_OBJECT_H
#define CHAT_DATA_WINDOW_AWARE_OBJECT_H

#include <QtCore/QList>

#include "aware-object.h"

class ChatDataWindow;

class KADUAPI ChatDataWindowAwareObject : public AwareObject<ChatDataWindowAwareObject>
{
protected:
	virtual void chatDataWindowCreated(ChatDataWindow *chatDataWindow) = 0;
	virtual void chatDataWindowDestroyed(ChatDataWindow *chatDataWindow) = 0;

public:
	static void notifyChatDataWindowCreated(ChatDataWindow *chatDataWindow);
	static void notifyChatDataWindowDestroyed(ChatDataWindow *chatDataWindow);

	void triggerAllChatDataWindowsCreated();
	void triggerAllChatDataWindowsDestroyed();

};

#endif // CHAT_DATA_WINDOW_AWARE_OBJECT_H
