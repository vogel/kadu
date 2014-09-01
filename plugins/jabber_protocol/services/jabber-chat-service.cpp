/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-file.h"
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
#include "resource/jabber-resource.h"
#include "services/raw-message-transformer-service.h"

#include "debug.h"

#include "iris-status-adapter.h"
#include "jabber-protocol.h"

#include "jabber-chat-service.h"

namespace XMPP
{

JabberChatService::JabberChatService(Account account, QObject *parent) :
		ChatService(account, parent)
{
	connect(ChatManager::instance(), SIGNAL(chatOpened(Chat)), this, SLOT(chatOpened(Chat)));
	connect(ChatManager::instance(), SIGNAL(chatClosed(Chat)), this, SLOT(chatClosed(Chat)));
}

JabberChatService::~JabberChatService()
{
	disconnect(ChatManager::instance(), 0, this, 0);
}

void JabberChatService::connectClient()
{
	connect(XmppClient.data(), SIGNAL(groupChatJoined(Jid)), this, SLOT(groupChatJoined(Jid)));
	connect(XmppClient.data(), SIGNAL(groupChatLeft(Jid)), this, SLOT(groupChatLeft(Jid)));
	connect(XmppClient.data(), SIGNAL(groupChatPresence(Jid,Status)), this, SLOT(groupChatPresence(Jid,Status)));
}

void JabberChatService::disconnectClient()
{
	disconnect(XmppClient.data(), 0, this, 0);
}

void JabberChatService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void JabberChatService::setXmppClient(Client *xmppClient)
{
	if (XmppClient)
		disconnectClient();

	XmppClient = xmppClient;

	if (XmppClient)
		connectClient();
}

int JabberChatService::maxMessageLength() const
{
	return 100000;
}

ChatDetailsRoom * JabberChatService::myRoomChatDetails(const Chat &chat) const
{
	if (chat.chatAccount() != account())
		return 0;

	return qobject_cast<ChatDetailsRoom *>(chat.details());
}

void JabberChatService::chatOpened(const Chat &chat)
{
	ChatDetailsRoom *details = myRoomChatDetails(chat);
	if (!details)
		return;

	OpenedRoomChats.insert(details->room(), chat);

	Jid jid = details->room();
	XmppClient.data()->groupChatJoin(jid.domain(), jid.node(), details->nick());
}

void JabberChatService::chatClosed(const Chat &chat)
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(ServiceAccount.protocolHandler());

	if (protocol)
	{
		protocol->resourcePool()->removeAllResources(chat.contacts().toContact().id());
	}

	ChatDetailsRoom *details = myRoomChatDetails(chat);
	if (!details)
		return;

	OpenedRoomChats.remove(details->room());
	ClosedRoomChats.insert(details->room(), chat);

	Jid jid = details->room();
	XmppClient.data()->groupChatLeave(jid.domain(), jid.node());
}

void JabberChatService::groupChatJoined(const Jid &jid)
{
	QString chatId = jid.bare();
	if (!OpenedRoomChats.contains(chatId))
		return;

	Chat chat = OpenedRoomChats.value(chatId);
	ChatDetailsRoom *details = myRoomChatDetails(chat);
	if (details)
		details->setConnected(true);
}

void JabberChatService::groupChatLeft(const Jid &jid)
{
	QString chatId = jid.bare();
	Chat chat;

	if (!ClosedRoomChats.contains(chatId))
	{
		if (!OpenedRoomChats.contains(chatId))
			return;
		chat = OpenedRoomChats.value(chatId);
	}
	else
		chat = ClosedRoomChats.value(chatId);

	ChatDetailsRoom *details = myRoomChatDetails(chat);
	if (details)
	{
		details->setConnected(false);

		ContactSet contacts = details->contacts();
		foreach (const Contact &contact, contacts)
			details->removeContact(contact);
	}

	ClosedRoomChats.remove(chatId);
}

void JabberChatService::groupChatPresence(const Jid &jid, const Status &status)
{
	Chat chat = OpenedRoomChats.value(jid.bare());

	ChatDetailsRoom *chatDetails = qobject_cast<ChatDetailsRoom *>(chat.details());
	if (!chatDetails)
		return;

	::Status contactStatus = IrisStatusAdapter::fromIrisStatus(status);
	Contact contact = ContactManager::instance()->byId(account(), jid.full(), ActionCreateAndAdd);

	if (!contactStatus.isDisconnected())
	{
		Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
		buddy.setDisplay(jid.resource());
		buddy.setTemporary(true);
	}

	contact.setCurrentStatus(contactStatus);

	if (contactStatus.isDisconnected())
		chatDetails->removeContact(contact);
	else
		chatDetails->addContact(contact);
}

XMPP::Jid JabberChatService::chatJid(const Chat &chat)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return XMPP::Jid();

	if (chatType->name() == "Contact")
	{
		ContactSet contacts = chat.contacts();
		Q_ASSERT(1 == contacts.size());

		XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(ServiceAccount.protocolHandler());

		if (protocol)
		{
			JabberResource *resource = protocol->resourcePool()->lockedJabberResource(contacts.toContact().id());
			if (resource)
			{
				return resource->jid().withResource(resource->resource().name());
			}
		}

		return contacts.toContact().id();
	}

	if (chatType->name() == "Room")
	{
		ChatDetailsRoom *details = qobject_cast<ChatDetailsRoom *>(chat.details());
		Q_ASSERT(details);

		return details->room();
	}

	return XMPP::Jid();
}

QString JabberChatService::chatMessageType(const Chat &chat, const XMPP::Jid &jid)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return QString();

	if (chatType->name() == "Room")
		return "groupchat";

	if (ContactMessageTypes.value(jid.bare()).isEmpty())
		return "chat";
	else
		return ContactMessageTypes.value(jid.bare());
}

bool JabberChatService::sendMessage(const ::Message &message)
{
	if (!XmppClient)
		return false;

	XMPP::Jid jid = chatJid(message.messageChat());
	if (jid.isEmpty())
		return false;

	XMPP::Message msg = XMPP::Message(jid);

	FormattedStringPlainTextVisitor plainTextVisitor;
	message.content()->accept(&plainTextVisitor);

	auto plain = plainTextVisitor.result();
	if (rawMessageTransformerService())
		plain = QString::fromUtf8(rawMessageTransformerService()->transform(plain.toUtf8(), {message}).rawContent());

	msg.setType(chatMessageType(message.messageChat(), jid));
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());
	msg.setFrom(XmppClient.data()->jid());

	emit messageAboutToSend(msg);
	XmppClient.data()->sendMessage(msg);

	return true;
}

bool JabberChatService::sendRawMessage(const Chat &chat, const QByteArray &rawMessage)
{
	if (!XmppClient)
		return false;

	XMPP::Jid jid = chatJid(chat);
	if (jid.isEmpty())
		return false;

	XMPP::Message msg = XMPP::Message(jid);

	msg.setType(chatMessageType(chat, jid));
	msg.setBody(rawMessage);
	msg.setTimeStamp(QDateTime::currentDateTime());
	msg.setFrom(XmppClient.data()->jid());

	emit messageAboutToSend(msg);
	XmppClient.data()->sendMessage(msg);

	return true;
}

void JabberChatService::handleReceivedMessage(const XMPP::Message &msg)
{
	if (!CurrentFormattedStringFactory)
		return;

	// skip empty messages
	if (msg.body().isEmpty())
		return;

	// skip messages with type error == Cancel (fixes bug #1642)
	if (msg.type() == "error")
		return;

	Chat chat;
	Contact contact;

	if (OpenedRoomChats.contains(msg.from().bare()))
	{
		chat = OpenedRoomChats.value(msg.from().bare());
		ChatDetailsRoom *details = myRoomChatDetails(chat);

		if (!details)
			return;

		if (msg.from().resource() == details->nick()) // message from myself
			return;

		contact = ContactManager::instance()->byId(account(), msg.from().full(), ActionCreateAndAdd);
		Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
		buddy.setDisplay(msg.from().resource());
		buddy.setTemporary(true);
	}
	else
	{
		contact = ContactManager::instance()->byId(account(), msg.from().bare(), ActionCreateAndAdd);
		chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);

		XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(ServiceAccount.protocolHandler());

		if (protocol)
		{
			// make sure current resource is in pool
			protocol->resourcePool()->addResource(msg.from().bare(), msg.from().resource());

			//if we have a locked resource, we simply talk to it
			JabberResource *resource = protocol->resourcePool()->lockedJabberResource(msg.from().bare());
			if (resource)
			{
				// if new resource appears, we remove locked resource, so that messages will be sent
				// to bare JID until some full JID talks and we lock to it then
				if (msg.from().resource() != resource->resource().name())
				{
					protocol->resourcePool()->removeLock(msg.from().bare());
				}
			}
			else
			{
				// first message from full JID - lock to this resource
				protocol->resourcePool()->lockToResource(msg.from().bare(), msg.from().resource());
			}
		}
	}

	::Message message = ::Message::create();
	message.setMessageChat(chat);
	message.setType(MessageTypeReceived);
	message.setMessageSender(contact);
	message.setSendDate(msg.timeStamp());
	message.setReceiveDate(QDateTime::currentDateTime());

	QString body = msg.body();
	if (rawMessageTransformerService())
		body = QString::fromUtf8(rawMessageTransformerService()->transform(body.toUtf8(), message).rawContent());

	auto formattedString = CurrentFormattedStringFactory.data()->fromPlainText(body);
	if (!formattedString || formattedString->isEmpty())
		return;

	message.setContent(std::move(formattedString));

	QString messageType = msg.type().isEmpty()
	        ? "message"
	        : msg.type();

	ContactMessageTypes.insert(msg.from().bare(), messageType);

	emit messageReceived(message);
}

}

#include "moc_jabber-chat-service.cpp"
