/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
	virtual bool sendMessage(const Chat &chat, FormattedMessage &formattedMessage, bool silent = false);

};

#endif // JABBER_CHAT_SERVICE_H
