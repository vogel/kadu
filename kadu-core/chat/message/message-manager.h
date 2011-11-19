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

class KADUAPI MessageManager : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	QMultiMap<Chat, Message> UnreadMessages;

	MessageManager();
	virtual ~MessageManager();

private slots:
	void messageReceivedSlot(const Message &message);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static MessageManager * instance();

	void addUnreadMessage(const Message &message);
	void removeUnreadMessage(const Message &message);

	void markAllMessagesAsRead(const Chat &chat);

signals:
	void messageReceived(const Message &message);
	void messageSent(const Message &message);

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

};

#endif // MESSAGE_MANAGER_H
