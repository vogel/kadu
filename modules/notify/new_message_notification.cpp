/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qstylesheet.h>

#include "new_message_notification.h"

void MessageNotification::registerEvents(Notify *manager)
{
	manager->registerEvent("NewChat", "New chat", CallbackNotRequired);
	manager->registerEvent("NewMessage", "New message", CallbackNotRequired);
}

void MessageNotification::unregisterEvents(Notify *manager)
{
	manager->unregisterEvent("NewChat");
	manager->unregisterEvent("NewMessage");
}

MessageNotification::MessageNotification(MessageType messageType, const UserListElements &userListElements, const QString &message)
	: Notification(messageType == NewChat ? "NewChat" : "NewMessage", "Message", userListElements)
{
	const UserListElement &ule = userListElements[0];
	QString syntax;

	if (messageType == NewChat)
	{
		setTitle("New chat");
		syntax = "Chat with <b>%1</b>";
	}
	else
	{
		setTitle("New message");
		syntax = "New message from <b>%1</b>";
	}

	setText(tr(syntax).arg(ule.altNick()));
	setDetails(message);
}

