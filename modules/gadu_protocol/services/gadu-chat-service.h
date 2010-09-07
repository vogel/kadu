/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	QString getContent(struct gg_event *e);
	bool ignoreRichText(struct gg_event *e, Contact sender);
	bool ignoreImages(struct gg_event *e, Contact sender);
	FormattedMessage createFormattedMessage(struct gg_event *e, Chat chat, Contact sender);

	void removeTimeoutUndeliveredMessages();

	void handleEventMsg(struct gg_event *e);
	void handleEventAck(struct gg_event *e);

public:
	GaduChatService(GaduProtocol *protocol);

public slots:
	virtual bool sendMessage(Chat chat, FormattedMessage &message, bool silent = false);

signals:
	/**
		Sygna� daje mozliwo�� operowania na wiadomo�ci
		kt�ra przysz�a z serwera jeszcze w jej oryginalnej
		formie przed konwersj� na unicode i innymi zabiegami.
		Tre�� wiadomo�ci mo�na zmieni� grzebi�c w buforze msg,
		ale uwaga: mo�na zepsu� formatowanie tekstu zapisane
		w formats. Oczywi�cie je r�wnie� mo�na zmienia� wed�ug
		opisu protoko�u GG ;)
		Mo�na te� przerwa� dalsz� obr�bk� wiadomo�ci ustawiaj�c
		stop na true.
	**/
	void filterRawIncomingMessage(Chat chat, Contact sender, QString &msg, bool &ignore);

};

#endif // GADU_CHAT_SERVICE_H
