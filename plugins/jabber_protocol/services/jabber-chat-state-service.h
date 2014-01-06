/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_CHAT_STATE_SERVICE_H
#define JABBER_CHAT_STATE_SERVICE_H

#include <QtCore/QHash>
#include <QtCore/QPointer>

#include "chat/chat.h"

#include "xmpp_message.h"

#include "protocols/services/chat-state-service.h"

class JabberProtocol;

namespace XMPP
{

class Client;

class JabberChatStateService : public ChatStateService
{
	Q_OBJECT

	struct ContactInfo
	{
		bool UserRequestedEvents;
		QString EventId;

		ChatState ContactChatState;
		ChatState LastChatState;

		ContactInfo() :
				UserRequestedEvents(false), ContactChatState(::XMPP::StateNone), LastChatState(::XMPP::StateNone)
		{
		}
	};

	QPointer<Client> XmppClient;

	QHash<Contact, ContactInfo> ContactInfos;

	bool shouldSendEvent(const Contact &contact);

	void setChatState(const Contact &contact, ChatState state);

	static State xmppStateToContactState(ChatState state);

public:
	explicit JabberChatStateService(Account account, QObject *parent = 0);
	virtual ~JabberChatStateService();

	virtual void sendState(const Contact &contact, State state);

	void setClient(Client *xmppClient);

public slots:
	void handleReceivedMessage(const Message &m);
	void handleMessageAboutToSend(Message &message);

};

}

#endif // JABBER_CHAT_STATE_SERVICE_H
