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

#ifndef CHATS_BUDDIES_SPLITTER_H
#define CHATS_BUDDIES_SPLITTER_H

#include <QtCore/QSet>
#include <QtCore/QVector>

class Buddy;
class Chat;
class Talkable;

class ChatsBuddiesSplitter
{
	QSet<Chat> UsedChats;

	QSet<Chat> Chats;
	QSet<Buddy> Buddies;

	void processChat(const Chat &chat);
	void assignChat(const Chat &chat);

public:
	ChatsBuddiesSplitter(QVector<Talkable> talkables);

	QSet<Chat> chats() const;
	QSet<Buddy> buddies() const;

};

#endif // CHATS_BUDDIES_SPLITTER_H
