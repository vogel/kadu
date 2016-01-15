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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>

class BuddyChatManager;
class BuddyManager;
class Buddy;
class ChatTypeManager;
class Chat;
class Talkable;

class ChatsBuddiesSplitter : public QObject
{
	Q_OBJECT

public:
	explicit ChatsBuddiesSplitter(QVector<Talkable> talkables, QObject *parent = nullptr);
	virtual ~ChatsBuddiesSplitter();

	QSet<Chat> chats() const;
	QSet<Buddy> buddies() const;

private:
	QPointer<BuddyChatManager> m_buddyChatManager;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatTypeManager> m_chatTypeManager;

	QVector<Talkable> m_talkables;
	QSet<Chat> UsedChats;

	QSet<Chat> Chats;
	QSet<Buddy> Buddies;

	void processChat(const Chat &chat);
	void assignChat(const Chat &chat);

private slots:
	INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_INIT void init();

};
