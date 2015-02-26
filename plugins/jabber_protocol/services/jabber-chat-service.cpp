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

#include "services/jabber-room-chat-service.h"
#include "iris-status-adapter.h"
#include "jabber-protocol.h"

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "gui/windows/message-dialog.h"
#include "message/message.h"
#include "message/raw-message.h"
#include "misc/misc.h"
#include "resource/jabber-resource-pool.h"
#include "services/raw-message-transformer-service.h"
#include "debug.h"

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

void JabberChatService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void JabberChatService::setRoomChatService(JabberRoomChatService *roomChatService)
{
	m_roomChatService = roomChatService;
}

int JabberChatService::maxMessageLength() const
{
	return 100000;
}

QString JabberChatService::chatJid(const Chat &chat) const
{
	auto chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return QString{};

	if (chatType->name() == "Contact")
	{
		auto contacts = chat.contacts();
		Q_ASSERT(1 == contacts.size());

		// auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());

		//if (protocol)
		//{
			//auto resource = protocol->resourcePool()->lockedJabberResource(contacts.toContact().id());
			//if (resource)
			//	return resource->jid().withResource(resource->resource().name());
		//}

		return contacts.toContact().id();
	}

	if (chatType->name() == "Room")
	{
		auto details = qobject_cast<ChatDetailsRoom *>(chat.details());
		Q_ASSERT(details);

		return details->room();
	}

	return QString{};
}

QXmppMessage::Type JabberChatService::chatMessageType(const Chat &chat, const QString &bareJid) const
{
	auto chatType = ChatTypeManager::instance()->chatType(chat.type());
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

	auto bareJid = chatJid(message.messageChat());
	if (bareJid.isEmpty())
		return false;

	auto xmppMessage = QXmppMessage{};

	FormattedStringPlainTextVisitor plainTextVisitor;
	message.content()->accept(&plainTextVisitor);

	auto plain = plainTextVisitor.result();
	if (rawMessageTransformerService())
		plain = QString::fromUtf8(rawMessageTransformerService()->transform(plain.toUtf8(), {message}).rawContent());

	xmppMessage.setBody(plain);
	xmppMessage.setFrom(m_client.data()->clientPresence().id());
	xmppMessage.setStamp(QDateTime::currentDateTime());
	xmppMessage.setTo(bareJid);
	xmppMessage.setType(chatMessageType(message.messageChat(), bareJid));

	// emit messageAboutToSend(msg);
	m_client.data()->sendPacket(xmppMessage);

	return true;
}

bool JabberChatService::sendRawMessage(const Chat &chat, const QByteArray &rawMessage)
{
	if (!m_client)
		return false;

	auto bareJid = chatJid(chat);
	if (bareJid.isEmpty())
		return false;

	auto xmppMessage = QXmppMessage{};

	xmppMessage.setBody(rawMessage);
	xmppMessage.setFrom(m_client.data()->clientPresence().id());
	xmppMessage.setStamp(QDateTime::currentDateTime());
	xmppMessage.setTo(bareJid);
	xmppMessage.setType(chatMessageType(chat, bareJid));

	// emit messageAboutToSend(msg);

	m_client.data()->sendPacket(xmppMessage);

	return true;
}

void JabberChatService::handleReceivedMessage(const QXmppMessage &xmppMessage)
{
	if (!m_formattedStringFactory)
		return;

	if (xmppMessage.body().isEmpty())
		return;

	if (xmppMessage.type() == QXmppMessage::Type::Error) // #1642
		return;

	// auto message = m_roomChatService->shouldHandleReceivedMessage(msg)
	// 	? m_roomChatService->handleReceivedMessage(msg)
// 		: handleNormalReceivedMessage(msg);
	auto message = handleNormalReceivedMessage(xmppMessage);
	if (message.isNull())
		return;

	message.setType(MessageTypeReceived);
	message.setSendDate(xmppMessage.stamp());
	message.setReceiveDate(QDateTime::currentDateTime());

	auto body = xmppMessage.body();
	if (rawMessageTransformerService())
		body = QString::fromUtf8(rawMessageTransformerService()->transform(body.toUtf8(), message).rawContent());

	auto htmlBody = replacedNewLine(Qt::escape(body), QLatin1String("<br/>"));
	auto formattedString = m_formattedStringFactory.data()->fromHtml(htmlBody);
	if (!formattedString || formattedString->isEmpty())
		return;

	message.setContent(std::move(formattedString));

	auto id = xmppMessage.from();
	auto resourceIndex = id.indexOf('/');
	if (resourceIndex >= 0)
		id = id.mid(0, resourceIndex);
	m_contactMessageTypes.insert(id, xmppMessage.type());

	emit messageReceived(message);
}

Message JabberChatService::handleNormalReceivedMessage(const QXmppMessage &xmppMessage)
{
	auto id = xmppMessage.from();
	auto resourceIndex = id.indexOf('/');
	if (resourceIndex >= 0)
		id = id.mid(0, resourceIndex);

	auto contact = ContactManager::instance()->byId(account(), id, ActionCreateAndAdd);
	auto chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);

	auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
	if (protocol)
	{
		// make sure current resource is in pool
		// protocol->resourcePool()->addResource(msg.from().bare(), msg.from().resource());

		//if we have a locked resource, we simply talk to it
		// auto resource = protocol->resourcePool()->lockedJabberResource(msg.from().bare());
		// if (resource)
		// {
			// if new resource appears, we remove locked resource, so that messages will be sent
			// to bare JID until some full JID talks and we lock to it then
			// if (msg.from().resource() != resource->resource().name())
				// protocol->resourcePool()->removeLock(msg.from().bare());
		//}
		//else
		//{
			// first message from full JID - lock to this resource
		//	protocol->resourcePool()->lockToResource(msg.from().bare(), msg.from().resource());
		//}
	}

	auto message = Message::create();
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
