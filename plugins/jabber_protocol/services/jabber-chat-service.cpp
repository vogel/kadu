/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QTextDocument>

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/chat-details-room.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "message/message.h"
#include "misc/misc.h"

#include "debug.h"
#include "html_document.h"

#include "iris-status-adapter.h"
#include "jabber-protocol.h"

#include "jabber-chat-service.h"

namespace XMPP
{

JabberChatService::JabberChatService(JabberProtocol *protocol) :
		ChatService(protocol)
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

void JabberChatService::setClient(Client *xmppClient)
{
	if (XmppClient)
		disconnectClient();

	XmppClient = xmppClient;

	if (XmppClient)
		connectClient();
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
		details->setConnected(false);

	ContactSet contacts = details->contacts();
	foreach (const Contact &contact, contacts)
		details->removeContact(contact);

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

bool JabberChatService::sendMessage(const Chat &chat, const QString &message, bool silent)
{
	if (!XmppClient)
		return false;

	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return false;

	QString jid;

	if (chatType->name() == "Contact")
	{
		ContactSet contacts = chat.contacts();
		Q_ASSERT(1 == contacts.size());

		jid = contacts.toContact().id();
	}
	else if (chatType->name() == "Room")
	{
		ChatDetailsRoom *details = qobject_cast<ChatDetailsRoom *>(chat.details());
		Q_ASSERT(details);

		jid = details->room();
	}
	else
		return false;

	QTextDocument document;
	/*
	 * If message does not contain < then we can assume that this is plain text. Some plugins, like
	 * encryption_ng, are using sendMessage() method to pass messages (like public keys). We want
	 * these messages to have proper lines and paragraphs.
	 */
	if (message.contains('<'))
		document.setHtml(message);
	else
		document.setPlainText(message);


	FormattedMessage formattedMessage = FormattedMessage::parse(&document);

	QString plain = formattedMessage.toPlain();

	kdebugmf(KDEBUG_INFO, "jabber: chat msg to %s body %s\n", qPrintable(jid), qPrintable(plain));
	const XMPP::Jid jus = jid;
	XMPP::Message msg = XMPP::Message(jus);

	bool stop = false;

	QByteArray data = plain.toUtf8();
	emit filterRawOutgoingMessage(chat, data, stop);
	plain = QString::fromUtf8(data);
	emit filterOutgoingMessage(chat, plain, stop);

	if (stop)
	{
		// TODO: implement formats
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	QString messageType = chatType->name() == "Room"
			? "groupchat"
			: !ContactMessageTypes.value(jus.bare()).isEmpty()
					? ContactMessageTypes.value(jus.bare())
					: "chat";

	msg.setType(messageType);
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());
	//msg.setFrom(jabberID);

	emit messageAboutToSend(msg);
	XmppClient.data()->sendMessage(msg);

	if (!silent)
	{
		::Message msg = ::Message::create();
		msg.setMessageChat(chat);
		msg.setType(MessageTypeSent);
		msg.setMessageSender(account().accountContact());
		msg.setContent(formattedMessage.toHtml()); // do not add encrypted message here
		msg.setSendDate(QDateTime::currentDateTime());
		msg.setReceiveDate(QDateTime::currentDateTime());

		emit messageSent(msg);
	}

	return true;
}

void JabberChatService::handleReceivedMessage(const XMPP::Message &msg)
{
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
	}

	bool ignore = false;

	QByteArray body = msg.body().toUtf8();
	emit filterRawIncomingMessage(chat, contact, body, ignore);

	FormattedMessage formattedMessage(QString::fromUtf8(body));

	QString plain = formattedMessage.toPlain();

	emit filterIncomingMessage(chat, contact, plain, ignore);
	if (ignore)
		return;

	QString messageType = msg.type().isEmpty()
	        ? "message"
	        : msg.type();

	ContactMessageTypes.insert(msg.from().bare(), messageType);

	HtmlDocument::escapeText(plain);

	::Message message = ::Message::create();
	message.setMessageChat(chat);
	message.setType(MessageTypeReceived);
	message.setMessageSender(contact);
	message.setContent(plain);
	message.setSendDate(msg.timeStamp());
	message.setReceiveDate(QDateTime::currentDateTime());

	emit messageReceived(message);
}

}
