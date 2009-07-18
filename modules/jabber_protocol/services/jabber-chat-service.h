/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CHAT_SERVICE_H
#define JABBER_CHAT_SERVICE_H

#include <im.h>
#include <xmpp.h>

#include "protocols/services/chat-service.h"

class Chat;
class JabberProtocol;

class JabberChatService : public ChatService
{
	Q_OBJECT

	JabberProtocol *Protocol;
private slots:
	void clientMessageReceived(const XMPP::Message &msg);

public:
	JabberChatService(JabberProtocol *protocol);

public slots:
	virtual bool sendMessage(Chat *chat, FormattedMessage &formattedMessage);
};

#endif // JABBER_CHAT_SERVICE_H
