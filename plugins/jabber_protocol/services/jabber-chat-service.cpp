/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "jabber-chat-service.h"

#include "services/jabber-chat-state-service.h"
#include "services/jabber-resource-service.h"
#include "services/jabber-room-chat-service.h"
#include "jabber-protocol.h"
#include "jid.h"

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message-storage.h"
#include "message/message.h"
#include "message/raw-message.h"
#include "misc/misc.h"
#include "services/raw-message-transformer-service.h"
#include "windows/message-dialog.h"

#include <QtGui/QTextDocument>
#include <qxmpp/QXmppMessage.h>

JabberChatService::JabberChatService(QXmppClient *client, Account account, QObject *parent) :
		ChatService{account, parent},
		m_client{client}
{
	connect(m_client, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(handleReceivedMessage(QXmppMessage)));
}

JabberChatService::~JabberChatService()
{
}

void JabberChatService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void JabberChatService::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void JabberChatService::setChatTypeManager(ChatTypeManager *chatTypeManager)
{
	m_chatTypeManager = chatTypeManager;
}

void JabberChatService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberChatService::setChatStateService(JabberChatStateService *chatStateService)
{
	m_chatStateService = chatStateService;
}

void JabberChatService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void JabberChatService::setMessageStorage(MessageStorage *messageStorage)
{
	m_messageStorage = messageStorage;
}

void JabberChatService::setResourceService(JabberResourceService *resourceService)
{
	m_resourceService = resourceService;
}

void JabberChatService::setRoomChatService(JabberRoomChatService *roomChatService)
{
	m_roomChatService = roomChatService;
}

int JabberChatService::maxMessageLength() const
{
	return 60000;
}

QXmppMessage::Type JabberChatService::chatMessageType(const Chat &chat, const QString &bareJid) const
{
	auto chatType = m_chatTypeManager->chatType(chat.type());
	if (!chatType)
		return QXmppMessage::QXmppMessage::Normal;

	if (chatType->name() == "Room")
		return QXmppMessage::GroupChat;

	if (m_contactMessageTypes.contains(bareJid))
		return m_contactMessageTypes.value(bareJid);
	else
		return QXmppMessage::Chat;
}

bool JabberChatService::sendMessage(const Message &message)
{
	if (!m_client)
		return false;

	auto jid = m_resourceService->bestChatJid(message.messageChat());
	if (jid.isEmpty())
		return false;

	auto xmppMessage = QXmppMessage{};
	auto formattedContent = m_formattedStringFactory->fromHtml(message.content());

	FormattedStringPlainTextVisitor plainTextVisitor;
	formattedContent->accept(&plainTextVisitor);

	auto plain = plainTextVisitor.result();
	if (rawMessageTransformerService())
		plain = QString::fromUtf8(rawMessageTransformerService()->transform(plain.toUtf8(), {message}).rawContent());

	xmppMessage.setBody(plain);
	xmppMessage.setFrom(m_client.data()->clientPresence().id());
	xmppMessage.setStamp(QDateTime::currentDateTime());
	xmppMessage.setTo(jid.full());
	xmppMessage.setType(chatMessageType(message.messageChat(), jid.bare()));

	m_client.data()->sendPacket(m_chatStateService->withSentChatState(xmppMessage));

	return true;
}

bool JabberChatService::sendRawMessage(const Chat &chat, const QByteArray &rawMessage)
{
	if (!m_client)
		return false;

	auto jid = m_resourceService->bestChatJid(chat);
	if (jid.isEmpty())
		return false;

	auto xmppMessage = QXmppMessage{};

	xmppMessage.setBody(rawMessage);
	xmppMessage.setFrom(m_client.data()->clientPresence().id());
	xmppMessage.setStamp(QDateTime::currentDateTime());
	xmppMessage.setTo(jid.full());
	xmppMessage.setType(chatMessageType(chat, jid.bare()));

	m_client.data()->sendPacket(m_chatStateService->withSentChatState(xmppMessage));

	return true;
}

void JabberChatService::handleReceivedMessage(const QXmppMessage &xmppMessage)
{
	if (!m_formattedStringFactory)
		return;

	m_chatStateService->extractReceivedChatState(xmppMessage);

	if (xmppMessage.body().isEmpty())
		return;

	if (xmppMessage.type() == QXmppMessage::Type::Error) // #1642
		return;

	auto message = xmppMessage.type() == QXmppMessage::GroupChat
		? m_roomChatService->handleReceivedMessage(xmppMessage)
		: handleNormalReceivedMessage(xmppMessage);
	if (message.isNull())
		return;

	message.setType(MessageTypeReceived);
	message.setSendDate(xmppMessage.stamp().toLocalTime());
	message.setReceiveDate(QDateTime::currentDateTime());

	auto body = xmppMessage.body();
	if (rawMessageTransformerService())
		body = QString::fromUtf8(rawMessageTransformerService()->transform(body.toUtf8(), message).rawContent());

	message.setContent(normalizeHtml(plainToHtml(body)));

	auto id = xmppMessage.from();
	auto resourceIndex = id.indexOf('/');
	if (resourceIndex >= 0)
		id = id.mid(0, resourceIndex);
	m_contactMessageTypes.insert(id, xmppMessage.type());

	emit messageReceived(message);
}

Message JabberChatService::handleNormalReceivedMessage(const QXmppMessage &xmppMessage)
{
	auto jid = Jid::parse(xmppMessage.from());

	auto contact = m_contactManager->byId(account(), jid.bare(), ActionCreateAndAdd);
	auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);

	contact.addProperty("jabber:chat-resource", jid.resource(), CustomProperties::NonStorable);

	auto message = m_messageStorage->create();
	message.setMessageChat(chat);
	message.setMessageSender(contact);

	return message;
}

void JabberChatService::leaveChat(const Chat& chat)
{
	if (m_roomChatService->isRoomChat(chat))
		m_roomChatService->leaveChat(chat);
}

#include "moc_jabber-chat-service.cpp"
