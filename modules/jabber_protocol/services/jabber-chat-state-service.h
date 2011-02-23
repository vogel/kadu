/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat.h"

#include "xmpp_message.h"

#include "protocols/services/chat-state-service.h"

class Chat;
class JabberProtocol;

class JabberChatStateService : public ChatStateService
{
	Q_OBJECT

	struct ChatInfo
	{
		bool UserRequestedEvents;
		QString EventId;

		XMPP::ChatState ContactChatState;
		XMPP::ChatState LastChatState;

		ChatInfo()
		{
			ContactChatState = XMPP::StateNone;
			LastChatState = XMPP::StateNone;
			UserRequestedEvents = false;
		}
	};

	QHash<Chat, ChatInfo> ChatInfos;

	JabberProtocol *Protocol;

	bool shouldSendEvent(const Chat &chat);

	void setChatState(const Chat &chat, XMPP::ChatState state);

	static ContactActivity xmppStateToContactState(XMPP::ChatState state);

private slots:
	void incomingMessage(const XMPP::Message &m);
	void messageAboutToSend(XMPP::Message &message);

public:
	JabberChatStateService(JabberProtocol *parent);

	virtual void composingStarted(const Chat &chat);
	virtual void composingStopped(const Chat &chat);

	virtual void chatWidgetClosed(const Chat &chat);
	virtual void chatWidgetActivated(const Chat &chat);
	virtual void chatWidgetDeactivated(const Chat &chat);
};

#endif // JABBER_CHAT_STATE_SERVICE_H
