/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QTextCodec>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/message/message-manager.h"
#include "chat/chat-details.h"
#include "chat/chat-details-aggregate.h"
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

void PendingMessagesManager::deletePendingMessagesForChat(const Chat &chat)
{
	QMutexLocker locker(&mutex());

	const QVector<Message> &messages = pendingMessagesForChat(chat);
	foreach (Message message, messages)
	{
		message.setPending(false);
		removeItem(message);
	}
}

Chat PendingMessagesManager::chatForBuddy(const Buddy &buddy)
{
	QMutexLocker locker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && buddy.contacts().contains(message.messageSender()))
			return message.messageChat();

	return Chat::null;
}

Chat PendingMessagesManager::chatForContact(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && message.messageSender() == contact)
			return message.messageChat();

	return Chat::null;
}

QVector<Message> PendingMessagesManager::pendingMessagesForContact(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	QVector<Message> result;

	foreach (const Message &message, items())
		if (message.isPending() && message.messageChat().contacts().contains(contact))
			result.append(message);

	return result;
}

QVector<Message> PendingMessagesManager::pendingMessagesForBuddy(const Buddy &buddy)
{
	QMutexLocker locker(&mutex());

	QVector<Message> result;
	QSet<Contact> contacts = buddy.contacts().toSet();

	foreach (const Message &message, items())
		if (message.isPending() && !message.messageChat().contacts().intersect(contacts).isEmpty())
			result.append(message);

	return result;
}

QVector<Message> PendingMessagesManager::pendingMessagesForChat(const Chat &chat)
{
	QMutexLocker locker(&mutex());

	QVector<Message> result;
	QSet<Chat> chats;

	ChatDetails *details = chat.details();
	ChatDetailsAggregate *aggregateDetails = qobject_cast<ChatDetailsAggregate *>(details);
	if (aggregateDetails)
		foreach (const Chat &ch, aggregateDetails->chats())
			chats.insert(ch);
	else
		chats.insert(chat);

	foreach (const Message &message, items())
		if (message.isPending() && chats.contains(message.messageChat()))
			result.append(message);

	return result;
}

QVector<Message> PendingMessagesManager::pendingMessages()
{
	QMutexLocker locker(&mutex());

	QVector<Message> result;

	foreach (const Message &message, items())
		if (message.isPending())
			result.append(message);

	return result;
}

Message PendingMessagesManager::firstPendingMessage()
{
	QMutexLocker locker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending())
			return message;

	return Message::null;
}

void PendingMessagesManager::itemAboutToBeAdded(Message message)
{
	// just ensure that owner buddy is managed - we need it to be shown on contact list
	BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

	emit messageAboutToBeAdded(message);
}

void PendingMessagesManager::itemAdded(Message message)
{
//	BuddyPreferredManager::instance()->updatePreferred(message.messageSender().ownerBuddy());
	emit messageAdded(message);
}

void PendingMessagesManager::itemAboutToBeRemoved(Message message)
{
	emit messageAboutToBeRemoved(message);
}

void PendingMessagesManager::itemRemoved(Message message)
{
//	BuddyPreferredManager::instance()->updatePreferred(message.messageSender().ownerBuddy());
	emit messageRemoved(message);
}

void PendingMessagesManager::loaded()
{
	SimpleManager<Message>::loaded();

	foreach (const Message &message, items())
	{
		// just ensure that all owner buddies are managed - we need them to be shown on contact list
		BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

		// each pending message is unread message of its chat
		MessageManager::instance()->addUnreadMessage(message);
	}
}
