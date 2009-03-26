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
#include "icons_manager.h"

#include "new_message_notification.h"

void MessageNotification::registerEvents()
{
	NotificationManager::instance()->registerEvent("NewChat", QT_TRANSLATE_NOOP("@default", "New chat"), CallbackNotRequired);
	NotificationManager::instance()->registerEvent("NewMessage", QT_TRANSLATE_NOOP("@default", "New message"), CallbackNotRequired);
}

void MessageNotification::unregisterEvents()
{
	NotificationManager::instance()->unregisterEvent("NewChat");
	NotificationManager::instance()->unregisterEvent("NewMessage");
}

MessageNotification::MessageNotification(MessageType messageType, const ContactList &contacts, const QString &message, Account *account)
	: AccountNotification(messageType == NewChat ? "NewChat" : "NewMessage", icons_manager->loadIcon("Message"), contacts, account)
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

