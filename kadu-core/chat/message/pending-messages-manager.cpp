/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
bool PendingMessagesManager::hasPendingMessagesForContact(const Contact &contact)
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && message.messageSender() == contact)
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessagesForBuddy(const Buddy &buddy)
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && buddy.contacts().contains(message.messageSender()))
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessagesForChat(const Chat &chat)
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && message.messageChat() == chat)
			return true;

	return false;
}

bool PendingMessagesManager::hasPendingMessages()
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending())
			return true;

	return false;
}

Chat PendingMessagesManager::chatForBuddy(const Buddy &buddy)
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && buddy.contacts().contains(message.messageSender()))
			return message.messageChat();

	return Chat::null;
}

Chat PendingMessagesManager::chatForContact(const Contact &contact)
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
		if (message.isPending() && message.messageSender() == contact)
			return message.messageChat();

	return Chat::null;
}

QList<Message> PendingMessagesManager::pendingMessagesForContact(const Contact &contact)
{
	QMutexLocker(&mutex());

	QList<Message> result;

	foreach (const Message &message, items())
		if (message.isPending() && message.messageChat().contacts().contains(contact))
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessagesForBuddy(const Buddy &buddy)
{
	QMutexLocker(&mutex());

	QList<Message> result;
	QSet<Contact> contacts = buddy.contacts().toSet();

	foreach (const Message &message, items())
		if (message.isPending() && !message.messageChat().contacts().intersect(contacts).isEmpty())
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessagesForChat(const Chat &chat)
{
	QMutexLocker(&mutex());

	QList<Message> result;

	foreach (const Message &message, items())
		if (message.isPending() && message.messageChat() == chat)
			result.append(message);

	return result;
}

QList<Message> PendingMessagesManager::pendingMessages()
{
	QMutexLocker(&mutex());

	QList<Message> result;

	foreach (const Message &message, items())
		if (message.isPending())
			result.append(message);

	return result;
}

Message PendingMessagesManager::firstPendingMessage()
{
	QMutexLocker(&mutex());

	foreach (const Message &message, items())
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
