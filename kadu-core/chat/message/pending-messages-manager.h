/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef PENDING_MESSAGES_MANAGER_H
#define PENDING_MESSAGES_MANAGER_H

#include <QtCore/QObject>

#include "chat/message/message.h"
#include "chat/message/message-shared.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Buddy;
class Chat;
class Contact;

/**
	Represents message queue awaiting to be read. Messages in this class can be also stored between alternate application launches.
	\class PendingMessagesManager
	\brief Pending messages queue.
**/
class KADUAPI PendingMessagesManager : public QObject, public SimpleManager<Message>
{
	Q_OBJECT
	Q_DISABLE_COPY(PendingMessagesManager)

	static PendingMessagesManager * Instance;

	PendingMessagesManager();
	virtual ~PendingMessagesManager();

protected:
	virtual void itemAboutToBeAdded(Message message);
	virtual void itemAdded(Message message);
	virtual void itemAboutToBeRemoved(Message message);
	virtual void itemRemoved(Message message);

public:
	static PendingMessagesManager * instance();

	virtual QString storageNodeName() { return QLatin1String("PendingMessages"); }
	virtual QString storageNodeItemName() { return QLatin1String("Message"); }

	bool hasPendingMessagesForContact(const Contact &contact);
	bool hasPendingMessagesForBuddy(const Buddy &buddy);
	bool hasPendingMessagesForChat(const Chat &chat);
	bool hasPendingMessages();

	Chat chatForBuddy(const Buddy &buddy);
	Chat chatForContact(const Contact &contact);

	QList<Message> pendingMessagesForContact(const Contact &contact);
	QList<Message> pendingMessagesForBuddy(const Buddy &buddy);
	QList<Message> pendingMessagesForChat(const Chat &chat);
	QList<Message> pendingMessages();

	Message firstPendingMessage();

signals:
	void messageAboutToBeAdded(Message message);
	void messageAdded(Message message);
	void messageAboutToBeRemoved(Message message);
	void messageRemoved(Message message);

};

#endif //PENDING_MESSAGES_MANAGER_H
