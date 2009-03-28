/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextDocument>

#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "icons_manager.h"

#include "new_message_notification.h"

NotifyEvent *MessageNotification::NewChatNotifyEvent = 0;
NotifyEvent *MessageNotification::NewMessageNotifyEvent = 0;

void MessageNotification::registerEvents()
{
	if (NewChatNotifyEvent)
		return;

	NewChatNotifyEvent = new NotifyEvent("NewChat", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "New chat"));
	NewMessageNotifyEvent = new NotifyEvent("NewMessage", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "New message"));

	NotificationManager::instance()->registerNotifyEvent(NewChatNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(NewMessageNotifyEvent);
}

void MessageNotification::unregisterEvents()
{
	NotificationManager::instance()->unregisterNotifyEvent(NewChatNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(NewMessageNotifyEvent);

	delete NewChatNotifyEvent;
	NewChatNotifyEvent = 0;

	delete NewMessageNotifyEvent;
	NewMessageNotifyEvent = 0;
}

MessageNotification::MessageNotification(MessageType messageType, const ContactList &contacts, const QString &message, Account *account)
	: AccountNotification(account, messageType == NewChat ? "NewChat" : "NewMessage", icons_manager->loadIcon("Message"), contacts)
{
	const Contact &contact = contacts[0];
	QString syntax;

	if (messageType == NewChat)
	{
		setTitle("New chat");
		syntax = tr("Chat with <b>%1</b>");
	}
	else
	{
		setTitle("New message");
		syntax = tr("New message from <b>%1</b>");
	}

	setText(syntax.arg(Qt::escape(contact.display())));
	setDetails(message);
}

