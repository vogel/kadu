/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class JabberPresenceService;

class BuddyManager;
class ContactManager;

class QXmppMucRoom;

class JabberRoomChat : public QObject
{
	Q_OBJECT

public:
	explicit JabberRoomChat(QXmppMucRoom *room, Chat chat, QObject *parent = nullptr);
	virtual ~JabberRoomChat();

	void setBuddyManager(BuddyManager *buddyManager);
	void setContactManager(ContactManager *contactManager);
	void setPresenceService(JabberPresenceService *presenceService);

	bool stayInRoomAfterClosingWindow() const;

	void join();
	void leave();

	Chat chat() const;
	QString nick() const;

signals:
	void joined(const Chat &chat);
	void left(const Chat &chat);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<JabberPresenceService> m_presenceService;

	QPointer<QXmppMucRoom> m_room;
	Chat m_chat;

private slots:
	void updated();
	void joined();
	void left();
	void participantChanged(const QString &id);
	void participantRemoved(const QString &id);

};
