/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include "configuration/configuration-file.h"
#include "debug.h"
#include "../tlen-protocol.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"

#include "tlen-chat-service.h"

TlenChatService::TlenChatService(TlenProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	//TODO 0.6.6:
	//move here some functions from tlen_protocol

	//connect(protocol, SIGNAL(ackReceived(int, uin_t, int)),
	//	this, SLOT(ackReceived(int, uin_t, int)));
       connect(protocol, SIGNAL(filterOutgoingMessage(Chat , QByteArray &, bool &)),
               this, SIGNAL(filterOutgoingMessage(Chat , QByteArray &, bool &)));
       connect(protocol, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)),
               this, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)));
       connect(protocol, SIGNAL(filterIncomingMessage(Chat , Contact, const QString &, time_t , bool &)),
               this, SIGNAL(filterIncomingMessage(Chat , Contact, const QString &, time_t, bool &)));
       connect(protocol, SIGNAL(messageReceived(const Message &)),
	       this, SIGNAL(messageReceived(const Message &)));
       connect(protocol, SIGNAL(messageSent(const Message &)),
	       this, SIGNAL(messageSent(const Message &)));
}

bool TlenChatService::sendMessage(Chat chat, FormattedMessage &message)
{
	kdebugf();
	return Protocol->sendMessage(chat, message);
}



