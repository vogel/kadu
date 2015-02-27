/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "protocols/services/chat-service.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <qxmpp/QXmppMessage.h>

class Chat;
class FormattedStringFactory;

class JabberChatStateService;
class JabberResourceService;
class JabberRoomChatService;
class Jid;

class QXmppClient;
class QXmppMessage;

class JabberChatService : public ChatService
{
	Q_OBJECT

public:
	explicit JabberChatService(QXmppClient *client, Account account, QObject *parent = nullptr);
	virtual ~JabberChatService();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void setChatStateService(JabberChatStateService *chatStateService);
	void setResourceService(JabberResourceService *resourceService);
	void setRoomChatService(JabberRoomChatService *roomChatService);

	virtual int maxMessageLength() const;

public slots:
	virtual bool sendMessage(const Message &message);
	virtual bool sendRawMessage(const Chat &chat, const QByteArray &rawMessage);

	/**
	 * @short If @p chat is a room chat, leave it. Otherwise do nothing.
	 */
	virtual void leaveChat(const Chat &chat);

	void handleReceivedMessage(const QXmppMessage &xmppMessage);

signals:
	void messageAboutToSend(Message &message);

private:
	QPointer<QXmppClient> m_client;
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<JabberChatStateService> m_chatStateService;
	QPointer<JabberResourceService> m_resourceService;
	QPointer<JabberRoomChatService> m_roomChatService;

	QMap<QString, QXmppMessage::Type> m_contactMessageTypes;

	QXmppMessage::Type chatMessageType(const Chat &chat, const QString &bareJid) const;
	Message handleNormalReceivedMessage(const QXmppMessage &xmppMessage);

};
