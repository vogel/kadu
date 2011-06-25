/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GADU_CHAT_SERVICE_H
#define GADU_CHAT_SERVICE_H

#include <libgadu.h>

#include "chat/chat.h"
#include "chat/message/message.h"
#include "protocols/services/chat-service.h"

class QTimer;

class GaduProtocol;

class GaduChatService : public ChatService
{
	Q_OBJECT

	QHash<int, Message> UndeliveredMessages;

	GaduProtocol *Protocol;

	friend class GaduProtocolSocketNotifiers;

	bool isSystemMessage(struct gg_event *e);
	Contact getSender(struct gg_event *e);
	bool ignoreSender(gg_event *e, Buddy sender);
	ContactSet getRecipients(struct gg_event *e);
	QByteArray getContent(struct gg_event *e);
	bool ignoreRichText(Contact sender);
	bool ignoreImages(Contact sender);
	FormattedMessage createFormattedMessage(struct gg_event *e, const QByteArray &content, Contact sender);

	void handleMsg(Contact sender, ContactSet recipients, MessageType type, struct gg_event *e);

	void handleEventMsg(struct gg_event *e);
	void handleEventMultilogonMsg(struct gg_event *e);
	void handleEventAck(struct gg_event *e);

	QTimer *RemoveTimer;

private slots:
	void removeTimeoutUndeliveredMessages();

public:
	GaduChatService(GaduProtocol *protocol);

public slots:
	virtual bool sendMessage(const Chat &chat, FormattedMessage &message, bool silent = false);

};

#endif // GADU_CHAT_SERVICE_H
