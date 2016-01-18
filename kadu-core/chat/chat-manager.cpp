/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-details-contact-set.h"
#include "chat/chat-details-contact.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-manager.h"
#include "message/unread-message-repository.h"

#include "chat-manager.h"

ChatManager::ChatManager(QObject *parent) :
		Manager<Chat>{parent}
{
}

ChatManager::~ChatManager()
{
}

void ChatManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void ChatManager::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

void ChatManager::init()
{
	m_configurationManager->registerStorableObject(this);

	foreach (const Message &message, m_unreadMessageRepository->allUnreadMessages())
		unreadMessageAdded(message);

	connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ChatManager::done()
{
	disconnect(m_unreadMessageRepository, 0, this, 0);

	foreach (const Message &message, m_unreadMessageRepository->allUnreadMessages())
		unreadMessageRemoved(message);

	m_configurationManager->unregisterStorableObject(this);
}

Chat ChatManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	return Chat::loadStubFromStorage(storagePoint);
}

void ChatManager::itemAboutToBeRegistered(Chat item)
{
	connect(item, SIGNAL(updated()), this, SLOT(chatDataUpdated()));
	connect(item, SIGNAL(opened()), this, SLOT(chatOpened()));
	connect(item, SIGNAL(closed()), this, SLOT(chatClosed()));

	emit chatAboutToBeAdded(item);
}

void ChatManager::itemRegistered(Chat item)
{
	emit chatAdded(item);
}

void ChatManager::itemAboutToBeUnregisterd(Chat item)
{
	disconnect(item, 0, this, 0);

	emit chatAboutToBeRemoved(item);
}

void ChatManager::itemUnregistered(Chat item)
{
	emit chatRemoved(item);
}

QVector<Chat> ChatManager::chats(const Account &account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Chat> chats;

	if (account.isNull())
		return chats;

	foreach (const Chat &chat, allItems())
		if (account == chat.chatAccount())
			chats.append(chat);

	return chats;
}

Chat ChatManager::byDisplay(const QString &display)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (display.isEmpty())
		return Chat::null;

	foreach (const Chat &chat, allItems())
		if (display == chat.display())
			return chat;

	return Chat::null;
}

void ChatManager::chatDataUpdated()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatUpdated(chat);
}

void ChatManager::chatOpened()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatOpened(chat);
}

void ChatManager::chatClosed()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatClosed(chat);
}

void ChatManager::unreadMessageAdded(const Message &message)
{
	const Chat &chat = message.messageChat();
	chat.setUnreadMessagesCount(chat.unreadMessagesCount() + 1);
}

void ChatManager::unreadMessageRemoved(const Message &message)
{
	const Chat &chat = message.messageChat();
	quint16 unreadMessagesCount = chat.unreadMessagesCount();
	if (unreadMessagesCount > 0)
		chat.setUnreadMessagesCount(unreadMessagesCount - 1);
}

#include "moc_chat-manager.cpp"
