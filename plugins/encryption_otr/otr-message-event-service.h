/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_MESSAGE_EVENT_SERVICE_H
#define OTR_MESSAGE_EVENT_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class ChatWidgetManager;
class Contact;

class OtrMessageEventService : public QObject
{
	Q_OBJECT

	QPointer<ChatWidgetManager> CurrentChatWidgetManager;

	QString messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const;
	QString gpgErrorString(gcry_error_t errorCode) const;

public:
	static void wrapperOtrHandleMessageEvent(void *data, OtrlMessageEvent event, ConnContext *context, const char *message, gcry_error_t error);

	Q_INVOKABLE OtrMessageEventService();
	virtual ~OtrMessageEventService();

	void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);

	void handleMessageEvent(const Contact &contact, OtrlMessageEvent event, const QString &message, gcry_error_t errorCode) const;

};

#endif // OTR_MESSAGE_EVENT_SERVICE_H
