/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_CHAT_SERVICE_H
#define JABBER_CHAT_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QWeakPointer>

#include <im.h>
#include <xmpp.h>

#include "protocols/services/chat-service.h"

class Chat;
class ChatDetailsRoom;
class JabberProtocol;

namespace XMPP
{

class Client;

class JabberChatService : public ChatService
{
	Q_OBJECT

	QWeakPointer<Client> XmppClient;

	QMap<QString, QString> ContactMessageTypes;
	QMap<QString, Chat> OpenedRoomChats;
 	QMap<QString, Chat> ClosedRoomChats;

	void connectClient();
	void disconnectClient();

	ChatDetailsRoom * myRoomChatDetails(const Chat &chat) const;

private slots:
	void chatOpened(const Chat &chat);
	void chatClosed(const Chat &chat);

	void groupChatJoined(const Jid &jid);
	void groupChatLeft(const Jid &jid);
	void groupChatPresence(const Jid &jid, const Status &status);

public:
	explicit JabberChatService(JabberProtocol *protocol);
	virtual ~JabberChatService();

	void setClient(Client *xmppClient);

public slots:
	virtual bool sendMessage(const Chat &chat, const QString &message, bool silent = false);

	void handleReceivedMessage(const Message &msg);

signals:
	void messageAboutToSend(Message &message);

};

}

#endif // JABBER_CHAT_SERVICE_H
