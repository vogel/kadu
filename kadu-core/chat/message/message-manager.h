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

class KADUAPI MessageManager : public QObject, public SimpleManager<Message>, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageManager)

	static MessageManager * Instance;

	MessageManager();
	virtual ~MessageManager();

private slots:
	void messageDataUpdated();

	void messageReceivedSlot(const Message &message);

protected:
	virtual bool shouldStore();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void itemAboutToBeAdded(Message item);
	virtual void itemAdded(Message item);
	virtual void itemAboutToBeRemoved(Message item);
	virtual void itemRemoved(Message item);

public:
	static MessageManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Messages"); }
	virtual QString storageNodeItemName() { return QLatin1String("Message"); }

signals:
	void messageReceived(const Message &message);
	void messageSent(const Message &message);

	void messageAboutToBeAdded(const Message &message);
	void messageAdded(const Message &message);
	void messageAboutToBeRemoved(const Message &message);
	void messageRemoved(const Message &message);

	void messageUpdated(const Message &message);

};

#endif // MESSAGE_MANAGER_H
