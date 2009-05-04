/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CHAT_SERVICE_H
#define TLEN_CHAT_SERVICE_H

#include "protocols/services/chat-service.h"

class Chat;
class TlenProtocol;

class TlenChatService : public ChatService
{
	Q_OBJECT

	TlenProtocol *Protocol;

public:
	TlenChatService(TlenProtocol *protocol);

public slots:
	virtual bool sendMessage(Chat *chat, Message &message);
};

#endif // TLEN_CHAT_SERVICE_H
