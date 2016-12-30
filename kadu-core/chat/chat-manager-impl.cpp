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
#include "chat/chat-storage.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-manager.h"
#include "message/unread-message-repository.h"

#include "chat-manager-impl.h"

ChatManagerImpl::ChatManagerImpl(QObject *parent) :
		ChatManager{parent}
{
}

ChatManagerImpl::~ChatManagerImpl()
{
}

void ChatManagerImpl::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void ChatManagerImpl::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void ChatManagerImpl::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

void ChatManagerImpl::init()
{
	m_configurationManager->registerStorableObject(this);

	foreach (const Message &message, m_unreadMessageRepository->allUnreadMessages())
		unreadMessageAdded(message);

	connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ChatManagerImpl::done()
{
	disconnect(m_unreadMessageRepository, 0, this, 0);

	foreach (const Message &message, m_unreadMessageRepository->allUnreadMessages())
		unreadMessageRemoved(message);

	m_configurationManager->unregisterStorableObject(this);
}

Chat ChatManagerImpl::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	return m_chatStorage->loadStubFromStorage(storagePoint);
}

void ChatManagerImpl::itemAboutToBeAdded(Chat item)
{
	ChatManager::itemAboutToBeAdded(item);

	connect(item, SIGNAL(updated()), this, SLOT(chatDataUpdated()));
	connect(item, SIGNAL(opened()), this, SLOT(chatOpened()));
	connect(item, SIGNAL(closed()), this, SLOT(chatClosed()));
}

void ChatManagerImpl::itemAboutToBeRemoved(Chat item)
{
	disconnect(item, nullptr, this, nullptr);

	ChatManager::itemAboutToBeRemoved(item);
}

QVector<Chat> ChatManagerImpl::chats(const Account &account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Chat> chats;

	if (account.isNull())
		return chats;

	foreach (const Chat &chat, items())
		if (account == chat.chatAccount())
			chats.append(chat);

	return chats;
}

Chat ChatManagerImpl::byDisplay(const QString &display)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (display.isEmpty())
		return Chat::null;

	foreach (const Chat &chat, items())
		if (display == chat.display())
			return chat;

	return Chat::null;
}

void ChatManagerImpl::chatDataUpdated()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit chatUpdated(chat);
}

void ChatManagerImpl::chatOpened()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit ChatManager::chatOpened(chat);
}

void ChatManagerImpl::chatClosed()
{
	Chat chat(sender());
	if (!chat.isNull())
		emit ChatManager::chatClosed(chat);
}

void ChatManagerImpl::unreadMessageAdded(const Message &message)
{
	const Chat &chat = message.messageChat();
	chat.setUnreadMessagesCount(chat.unreadMessagesCount() + 1);
}

void ChatManagerImpl::unreadMessageRemoved(const Message &message)
{
	const Chat &chat = message.messageChat();
	quint16 unreadMessagesCount = chat.unreadMessagesCount();
	if (unreadMessagesCount > 0)
		chat.setUnreadMessagesCount(unreadMessagesCount - 1);
}

#include "moc_chat-manager-impl.cpp"
