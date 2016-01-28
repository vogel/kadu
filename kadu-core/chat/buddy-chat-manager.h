/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "exports.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class Buddy;
class ChatManager;
class ChatStorage;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class BuddyChatManager
 * @short Manager buddy chats from contact chats.
 *
 * This class is responsible for creating, deleting and updating buddy chats that are created from
 * contact chats of every buddy.
 *
 * If contact is added or removed from buddy then its buddy chat is automatically updated.
 */
class KADUAPI BuddyChatManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyChatManager(QObject *parent = nullptr);
	virtual ~BuddyChatManager();

	/**
	 * @short Return 'Buddy' chat from given Chat.
	 * @param chat chat to transform to 'Buddy' chat
	 * @return 'Buddy' chat from given Chat
	 *
	 * If chat is of type 'Buddy' then it is returned. If chat is of type 'Contact' then
	 * buddy chat with chat's contact's owner buddy is returned. For any other chat types
	 * Chat::Null is returned. No chats are added to ChatManager by this method.
	 */
	Chat buddyChat(const Chat &chat);

	/**
	 * @short Return BuddyChat for given Buddy.
	 * @param buddy buddy to get 'Buddy' chat from
	 * @return 'Buddy' chat for given Buddy
	 *
	 * This method returns Chat of type 'Buddy' for given buddy. If will contain all of 'Contact'
	 * chats for this buddy's contacts. Chat::null is returned only if buddy is null. Otherwise,
	 * existing chat is returned or new one is created. No chats are added to ChatManager by this method.
	 */
	Chat buddyChat(const Buddy &buddy);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QHash<Buddy, Chat> BuddyChats;

	Chat createAndInsertBuddyChat(const Buddy &buddy);

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	/**
	 * @short Slot called when a contact is added to a buddy.
	 * @param buddy modified buddy
	 * @param contact added contact
	 *
	 * This slot called when a contact is added to a buddy. If 'Contact' chat exists for this contact
	 * and 'Buddy' chat exists for this buddy, then 'Contact' chat is added to 'Buddy' one.
	 */
	void buddyContactAdded(const Buddy &buddy, const Contact &contact);

	/**
	 * @short Slot called when a contact is removed from a buddy.
	 * @param buddy modified buddy
	 * @param contact removed contact
	 *
	 * This slot called when a contact is removed from a buddy. If 'Contact' chat exists for this contact
	 * and 'Buddy' chat exists for this buddy, then 'Contact' chat is removed from 'Buddy' one.
	 */
	void buddyContactRemoved(const Buddy &buddy, const Contact &contact);

	/**
	 * @short Slot called when a chat is added to ChatManager.
	 * @param addedChat newly added chat
	 *
	 * This slot called when a chat is added to ChatManager. If added chat is of type 'Contact'
	 * and exsiting 'Buddy' chat for this contact is found, then new chat it is added to
	 * 'Buddy' chat.
	 */
	void chatAdded(const Chat &addedChat);

	/**
	 * @short Slot called when a chat is removed from ChatManager.
	 * @param removedChat removed chat
	 *
	 * This slot called when a chat is removed from ChatManager. If removed chat is of type 'Contact'
	 * and exsiting 'Buddy' chat for this contact is found, then this chat it is removed from
	 * 'Buddy' chat.
	 */
	void chatRemoved(const Chat &removedChat);

};

/**
 * @}
 */
