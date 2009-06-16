/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/ignored-helper.h"

#include "configuration/configuration-file.h"
#include "debug.h"
#include "../tlen_protocol.h"
#include "message_box.h"
#include "misc/misc.h"

#include "tlen-chat-service.h"

TlenChatService::TlenChatService(TlenProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	//TODO 0.6.6:
	//move here some functions from tlen_protocol

	//connect(protocol, SIGNAL(ackReceived(int, uin_t, int)),
	//	this, SLOT(ackReceived(int, uin_t, int)));
       connect(protocol, SIGNAL(sendMessageFiltering(Chat *, QByteArray &, bool &)),
               this, SIGNAL(sendMessageFiltering(Chat *, QByteArray &, bool &)));
       connect(protocol, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)),
               this, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)));
       connect(protocol, SIGNAL(receivedMessageFilter(Chat *, Contact, const QString &, time_t , bool &)),
               this, SIGNAL(receivedMessageFilter(Chat *, Contact, const QString &, time_t, bool &)));
       connect(protocol, SIGNAL(messageReceived(const Message &)),
	       this, SIGNAL(messageReceived(const Message &)));
       connect(protocol, SIGNAL(messageSent(const Message &)),
	       this, SIGNAL(messageSent(const Message &)));
}

bool TlenChatService::sendMessage(Chat *chat, FormattedMessage &message)
{
	kdebugf();
	return Protocol->sendMessage(chat, message);
}



