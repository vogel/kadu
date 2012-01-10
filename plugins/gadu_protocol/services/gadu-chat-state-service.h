/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef GADU_CHAT_STATE_SERVICE_H
#define GADU_CHAT_STATE_SERVICE_H

#include <libgadu.h>

#include "message/message.h"

#include "protocols/services/chat-state-service.h"

class ChatService;
class GaduProtocol;

class GaduChatStateService : public ChatStateService
{
	Q_OBJECT

	ChatService *CurrentChatService;
	gg_session *GaduSession;

	bool shouldSendEvent();

private slots:
	void messageReceived(const Message & message);

public:
	explicit GaduChatStateService(GaduProtocol *parent);
	virtual ~GaduChatStateService();

	virtual void sendState(const Contact &contact, State state);

	void setChatService(ChatService *chatService);
	void setGaduSession(gg_session *gaduSession);

public slots:
	void typingNotifyEventReceived(struct gg_event *e);

};

#endif // GADU_CHAT_STATE_SERVICE_H
