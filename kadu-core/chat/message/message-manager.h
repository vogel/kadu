/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "chat/message/message.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Buddy;

class KADUAPI MessageManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	QList<Message> UnreadMessages;

	MessageManager();
	virtual ~MessageManager();

	void init();

	bool importFromPendingMessages();

private slots:
	void messageReceivedSlot(const Message &message);

protected:
	virtual void load();
	virtual void store();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static MessageManager * instance();

	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("Messages"); }

	void addUnreadMessage(const Message &message);
	void removeUnreadMessage(const Message &message);

	const QList<Message> & allUnreadMessages() const;
	QList<Message> chatUnreadMessages(const Chat &chat) const;

	bool hasUnreadMessages() const;
	quint16 unreadMessagesCount() const;

	void markAllMessagesAsRead(const Chat &chat);

	Message unreadMessage() const;
	Message unreadMessageForBuddy(const Buddy &buddy) const;
	Message unreadMessageForContact(const Contact &contact) const;

signals:
	void messageReceived(const Message &message);
	void messageSent(const Message &message);

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

};

#endif // MESSAGE_MANAGER_H
