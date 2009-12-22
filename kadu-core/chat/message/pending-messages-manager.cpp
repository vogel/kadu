/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-list-configuration-helper.h"
#include "buddies/buddy-shared.h"
#include "chat/message/message-shared.h"
#include "chat/chat-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "pending-messages-manager.h"

PendingMessagesManager * PendingMessagesManager::Instance = 0;

PendingMessagesManager * PendingMessagesManager::instance()
{
	if (0 == Instance)
		Instance = new PendingMessagesManager();
	
	return Instance;
}

PendingMessagesManager::PendingMessagesManager()
{
}

PendingMessagesManager::~PendingMessagesManager()
{
}

// TODO: optimize
bool PendingMessagesManager::hasPendingMessagesForContact(Contact contact)
{
	foreach (Message message, items())
		if (message.isPending() && message.messageChat().contacts().contains(contact))
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessagesForBuddy(Buddy buddy)
{
	QSet<Contact> contacts = buddy.contacts().toSet();

	foreach (Message message, items())
		if (message.isPending() && !message.messageChat().contacts().intersect(contacts).isEmpty())
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessagesForChat(Chat chat)
{
	foreach (Message message, items())
		if (message.isPending() && message.messageChat() == chat)
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessages()
{
	foreach (Message message, items())
		if (message.isPending())
			return true;

	return false;
}

QList<Message> PendingMessagesManager::pendingMessagesForContact(Contact contact)
{
	QList<Message> result;

	foreach (Message message, items())
		if (message.isPending() && message.messageChat().contacts().contains(contact))
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessagesForBuddy(Buddy buddy)
{
	QList<Message> result;
	QSet<Contact> contacts = buddy.contacts().toSet();

	foreach (Message message, items())
		if (message.isPending() && !message.messageChat().contacts().intersect(contacts).isEmpty())
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessagesForChat(Chat chat)
{
	QList<Message> result;

	foreach (Message message, items())
		if (message.isPending() && message.messageChat() == chat)
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessages()
{
	QList<Message> result;

	foreach (Message message, items())
		if (message.isPending())
			result.append(message);

	return result;
}

Message PendingMessagesManager::firstPendingMessage()
{
	foreach (Message message, items())
		if (message.isPending())
			return message;

	return Message::null;
}

void PendingMessagesManager::itemAboutToBeAdded(Message message)
{
	emit messageAboutToBeAdded(message);
}

void PendingMessagesManager::itemAdded(Message message)
{
	emit messageAdded(message);
}

void PendingMessagesManager::itemAboutToBeRemoved(Message message)
{
	emit messageAboutToBeRemoved(message);
}

void PendingMessagesManager::itemRemoved(Message message)
{
	emit messageRemoved(message);
}

// void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
// {
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
// }
