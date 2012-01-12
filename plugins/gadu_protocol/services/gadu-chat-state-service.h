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

class GaduChatStateService : public ChatStateService
{
	Q_OBJECT

	gg_session *GaduSession;

	bool SendTypingNotifications;

public:
	explicit GaduChatStateService(Protocol *parent);
	virtual ~GaduChatStateService();

	virtual void sendState(const Contact &contact, State state);

	void setSendTypingNotifications(bool sendTypingNotifications);

public slots:
	void setGaduSession(gg_session *gaduSession);

	void handleEventTypingNotify(struct gg_event *e);

	void messageReceived(const Message & message);

};

#endif // GADU_CHAT_STATE_SERVICE_H
