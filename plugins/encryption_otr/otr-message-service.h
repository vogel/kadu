/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef OTR_MESSAGE_SERVICE_H
#define OTR_MESSAGE_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

extern "C" {
#	include <libotr/proto.h>
}

class Account;
class Contact;
class MessageManager;

class OtrMessageService : public QObject
{
	Q_OBJECT

	QPointer<MessageManager> CurrentMessageManager;

public:
	static void wrapperOtrInjectMessage(void *data, const char *accountName, const char *protocol, const char *recipient, const char *message);
	static int wrapperOtrMaxMessageSize(void *data, ConnContext *context);
	static const char * wrapperOtrResentMessagePrefix(void *data, ConnContext *context);
	static void wrapperOtrResentMessagePrefixFree(void *data, const char *prefix);

	explicit OtrMessageService(QObject *parent = 0);
	virtual ~OtrMessageService();

	void setMessageManager(MessageManager *messageManager);

	void injectMessage(const Contact &contact, const QByteArray &message) const;
	int maxMessageSize(const Account &account) const;
	QString resentMessagePrefix() const;

};

#endif // OTR_MESSAGE_SERVICE_H
