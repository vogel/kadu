/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message.h"
#include "protocols/services/account-service.h"

#include <QtCore/QPointer>
#include <im.h>
#include <xmpp.h>

class BuddyManager;
class Chat;
class ChatDetailsRoom;
class ChatManager;
class ContactManager;

Q_DECLARE_METATYPE(XMPP::Status)

namespace XMPP
{

class Client;

class JabberRoomChatService : public AccountService
{
	Q_OBJECT

public:
	explicit JabberRoomChatService(Account account, QObject *parent = nullptr);
	virtual ~JabberRoomChatService();

	void setBuddyManager(BuddyManager *buddyManager);
	void setChatManager(ChatManager *chatManager);
	void setContactManager(ContactManager *contactManager);

	void setXmppClient(Client *xmppClient);

	bool shouldHandleReceivedMessage(const XMPP::Message &msg) const;
	::Message handleReceivedMessage(const XMPP::Message &msg) const;

	bool isRoomChat(const Chat &chat) const;
	void leaveChat(const Chat &chat);

private slots:
	void chatOpened(const Chat &chat);
	void chatClosed(const Chat &chat);

	void groupChatJoined(const Jid &jid);
	void groupChatLeft(const Jid &jid);
	void groupChatPresence(const Jid &jid, const Status &status);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ContactManager> m_contactManager;

	QPointer<Client> m_client;

	QMap<QString, Chat> m_openedRoomChats;
 	QMap<QString, Chat> m_closedRoomChats;

	ChatDetailsRoom * myRoomChatDetails(const Chat &chat) const;

};

}
