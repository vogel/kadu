/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "talkable-converter.h"

#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-set.h"
#include "status/status-container.h"
#include "talkable/talkable.h"

TalkableConverter::TalkableConverter(QObject *parent) :
		QObject{parent}
{
}

TalkableConverter::~TalkableConverter()
{
}

void TalkableConverter::setBuddyChatManager(BuddyChatManager *buddyChatManager)
{
	m_buddyChatManager = buddyChatManager;
}

void TalkableConverter::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void TalkableConverter::setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager)
{
	m_buddyPreferredManager = buddyPreferredManager;
}

void TalkableConverter::setChatDataExtractor(ChatDataExtractor *chatDataExtractor)
{
	m_chatDataExtractor = chatDataExtractor;
}

void TalkableConverter::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

Account TalkableConverter::toAccount(const Talkable &talkable) const
{
	switch (talkable.type())
	{
		case Talkable::ItemChat: return talkable.chat().chatAccount();
		default:
			return toContact(talkable).contactAccount();
	}
}

Avatar TalkableConverter::toAvatar(const Talkable &talkable) const
{
	Avatar avatar;
	if (Talkable::ItemBuddy == talkable.type())
		avatar = toBuddy(talkable).buddyAvatar();

	if (!avatar || avatar.pixmap().isNull())
		avatar = toContact(talkable).avatar(true);

	return avatar;
}

Buddy TalkableConverter::toBuddy(const Talkable &talkable) const
{
	switch (talkable.type())
	{
		case Talkable::ItemBuddy: return talkable.buddy();
		case Talkable::ItemContact: return m_buddyManager->byContact(talkable.contact(), ActionCreateAndAdd);
		case Talkable::ItemChat: return m_buddyManager->byContact(toContact(talkable), ActionCreateAndAdd);
		default:
			return Buddy::null;
	}
}

Contact TalkableConverter::toContact(const Talkable &talkable) const
{
	switch (talkable.type())
	{
		case Talkable::ItemBuddy: return m_buddyPreferredManager->preferredContact(talkable.buddy());
		case Talkable::ItemContact: return talkable.contact();
		case Talkable::ItemChat:
			if (talkable.chat().contacts().size() == 1)
				return *talkable.chat().contacts().begin();
			else
				return Contact::null;
		default:
			return Contact::null;
	}
}

Chat TalkableConverter::toChat(const Talkable &talkable) const
{
	switch (talkable.type())
	{
		case Talkable::ItemBuddy:
		{
			auto const &chat = ChatTypeContact::findChat(m_chatManager, m_buddyPreferredManager->preferredContact2(talkable.buddy()), ActionCreateAndAdd);
			auto const &buddyChat = m_buddyChatManager->buddyChat(chat);
			return buddyChat ? buddyChat : chat;
		}
		case Talkable::ItemContact: return ChatTypeContact::findChat(m_chatManager, talkable.contact(), ActionCreateAndAdd);
		case Talkable::ItemChat: return talkable.chat();
		default:
			return Chat::null;
	}
}

QString TalkableConverter::toDisplay(const Talkable &talkable) const
{
	switch (talkable.type())
	{
		case Talkable::ItemBuddy: return talkable.buddy().display();
		case Talkable::ItemContact: return talkable.contact().display(true);
		case Talkable::ItemChat: return m_chatDataExtractor->data(talkable.chat(), Qt::DisplayRole).toString();
		default:
			return QString();
	}
}

Status TalkableConverter::toStatus(const Talkable &talkable) const
{
	if (talkable.isValidChat())
		return talkable.chat().chatAccount().statusContainer() ? talkable.chat().chatAccount().statusContainer()->status() : Status();
	else
		return toContact(talkable).currentStatus();
}
