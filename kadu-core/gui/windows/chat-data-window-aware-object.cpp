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

#include "gui/windows/chat-data-window.h"

#include "chat-data-window-aware-object.h"

KADU_AWARE_CLASS(ChatDataWindowAwareObject)

void ChatDataWindowAwareObject::notifyChatDataWindowCreated(ChatDataWindow *chatDataWindow)
{
	foreach (ChatDataWindowAwareObject *object, Objects)
		object->chatDataWindowCreated(chatDataWindow);
}

void ChatDataWindowAwareObject::notifyChatDataWindowDestroyed(ChatDataWindow *chatDataWindow)
{
	foreach (ChatDataWindowAwareObject *object, Objects)
		object->chatDataWindowDestroyed(chatDataWindow);
}

void ChatDataWindowAwareObject::triggerAllChatDataWindowsCreated()
{
	foreach (ChatDataWindow *chatDataWindow, ChatDataWindow::instances())
		chatDataWindowCreated(chatDataWindow);
}

void ChatDataWindowAwareObject::triggerAllChatDataWindowsDestroyed()
{
	foreach (ChatDataWindow *chatDataWindow, ChatDataWindow::instances())
		chatDataWindowDestroyed(chatDataWindow);
}
