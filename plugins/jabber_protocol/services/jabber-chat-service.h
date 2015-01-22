/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <im.h>
#include <xmpp.h>
#include <QtCore/QMap>
#include <QtCore/QPointer>

class Chat;
class FormattedStringFactory;

namespace XMPP
{

class Client;
class JabberRoomChatService;

class JabberChatService : public ChatService
{
	Q_OBJECT

public:
	explicit JabberChatService(Account account, QObject *parent = nullptr);
	virtual ~JabberChatService();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void setXmppClient(Client *xmppClient);
	void setRoomChatService(JabberRoomChatService *roomChatService);

	virtual int maxMessageLength() const;

public slots:
	virtual bool sendMessage(const ::Message &message);
	virtual bool sendRawMessage(const Chat &chat, const QByteArray &rawMessage);

	/**
	 * @short If @p chat is a room chat, leave it. Otherwise do nothing.
	 */
	virtual void leaveChat(const Chat &chat);

	void handleReceivedMessage(const Message &msg);

signals:
	void messageAboutToSend(Message &message);

private:
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<Client> m_client;
	QPointer<JabberRoomChatService> m_roomChatService;

	QMap<QString, QString> m_contactMessageTypes;

	XMPP::Jid chatJid(const Chat &chat);
	QString chatMessageType(const Chat &chat, const XMPP::Jid &jid);
	::Message handleNormalReceivedMessage(const Message &msg);

};

}
