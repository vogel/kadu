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
		ChatService(protocol), XmppClient(0)
{
}

JabberChatService::~JabberChatService()
{
}

void JabberChatService::connectClient()
{
	connect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));
	connect(XmppClient, SIGNAL(groupChatJoined(Jid)), this, SLOT(groupChatJoined(Jid)));
	connect(XmppClient, SIGNAL(groupChatPresence(Jid,Status)), this, SLOT(groupChatPresence(Jid,Status)));
}

void JabberChatService::disconnectClient()
{
	disconnect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));
	disconnect(XmppClient, SIGNAL(groupChatJoined(Jid)), this, SLOT(groupChatJoined(Jid)));
	disconnect(XmppClient, SIGNAL(groupChatPresence(Jid,Status)), this, SLOT(groupChatPresence(Jid,Status)));
}

void JabberChatService::clientDestroyed()
{
	XmppClient = 0;
}

void JabberChatService::setClient(Client *xmppClient)
{
	if (XmppClient)
		disconnectClient();

	XmppClient = xmppClient;

	if (XmppClient)
		connectClient();
}

void JabberChatService::groupChatJoined(const Jid &jid)
{
	printf("properly joined group chat: %s\n", qPrintable(jid.full()));
}

void JabberChatService::groupChatPresence(const Jid &jid, const Status &status)
{
	printf("status: %d\n", status.type());

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
	}

	contact.setCurrentStatus(contactStatus);

	if (contactStatus.isDisconnected())
		chatDetails->removeContact(contact);
	else
		chatDetails->addContact(contact);
}

bool JabberChatService::sendMessageToContactChat(const Chat &chat, const QString &message, bool silent)
{
	ContactSet contacts = chat.contacts();
	// TODO send to more users
	if (contacts.count() > 1 || contacts.count() == 0)
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

	//QString cleanmsg = toPlainText(mesg);
	QString plain = document.toPlainText();
	QString jid = contacts.toContact().id();
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

	QString messageType = false == ContactMessageTypes.value(jus.bare()).isEmpty()
	        ? ContactMessageTypes.value(jus.bare())
	        : "chat";

	msg.setType(messageType);
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());
	//msg.setFrom(jabberID);

	emit messageAboutToSend(msg);
	XmppClient->sendMessage(msg);

	if (!silent)
	{
		HtmlDocument::escapeText(plain);

		::Message message = ::Message::create();
		message.setMessageChat(chat);
		message.setType(MessageTypeSent);
		message.setMessageSender(account().accountContact());
		message.setContent(document.toPlainText()); // do not add encrypted message here
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		emit messageSent(message);
	}

	return true;
}

bool JabberChatService::sendMessageToRoomChat(const Chat &chat, const QString &message, bool silent)
{
	Q_UNUSED(message);
	Q_UNUSED(silent);

	ChatDetailsRoom *chatDetails = qobject_cast<ChatDetailsRoom *>(chat.details());
	if (!chatDetails)
		return false;

	XmppClient->groupChatJoin(chatDetails->server(), chatDetails->roomName(), account().id());

	QString chatId = QString("%1@%2").arg(chatDetails->roomName()).arg(chatDetails->server());
	OpenedRoomChats.insert(chatId, chat);

	Jid jid = chatId;
	XMPP::Message msg = XMPP::Message(jid);

	bool stop = false;

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

	//QString cleanmsg = toPlainText(mesg);
	QString plain = document.toPlainText();

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

	QString messageType = false == ContactMessageTypes.value(jid.bare()).isEmpty()
	        ? ContactMessageTypes.value(jid.bare())
	        : "chat";

	msg.setType(messageType);
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());

	emit messageAboutToSend(msg);
	XmppClient->sendMessage(msg);

	if (!silent)
	{
		HtmlDocument::escapeText(plain);

		::Message message = ::Message::create();
		message.setMessageChat(chat);
		message.setType(MessageTypeSent);
		message.setMessageSender(account().accountContact());
		message.setContent(document.toPlainText()); // do not add encrypted message here
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		emit messageSent(message);
	}

	return true;
}

bool JabberChatService::sendMessage(const Chat &chat, const QString &message, bool silent)
{
	if (!XmppClient)
		return false;

	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return false;

	if (chatType->name() == "Contact")
		return sendMessageToContactChat(chat, message, silent);
	if (chatType->name() == "Room")
		return sendMessageToRoomChat(chat, message, silent);

	return false;
}

void JabberChatService::handleContactChatReceivedMessage(const Message &msg)
{
	// skip empty messages
	if (msg.body().isEmpty())
		return;

	// skip messages with type error == Cancel (fixes bug #1642)
	if (msg.type() == "error")
		return;

	Contact contact = ContactManager::instance()->byId(account(), msg.from().bare(), ActionCreateAndAdd);
	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
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

void JabberChatService::handleRoomChatReceivedMessage(const Message &msg)
{
	// skip empty messages
	if (msg.body().isEmpty())
		return;

	printf("received message from %s %s of %s\n", qPrintable(msg.nick()), qPrintable(msg.from().full()),
		   qPrintable(msg.body()));
	MUCDecline decline = msg.mucDecline();
	printf("decline: %s %s\n", qPrintable(decline.from().full()), qPrintable(decline.reason()));
	QString password = msg.mucPassword();
	printf("password: %s\n", qPrintable(password));
	QList<MUCInvite> invites = msg.mucInvites();
	foreach (MUCInvite invite, invites)
		printf("invite: %s %s\n", qPrintable(invite.from().bare()), qPrintable(invite.reason()));
	QList<int> statuses = msg.getMUCStatuses();
	foreach (int status, statuses)
		printf("muc status: %d\n", status);

	if (!OpenedRoomChats.contains(msg.from().bare()))
		return;

	Chat chat = OpenedRoomChats.value(msg.from().bare());

	Contact contact = ContactManager::instance()->byId(account(), msg.from().full(), ActionCreateAndAdd);
	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(msg.from().resource());

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

void JabberChatService::handleReceivedMessage(const XMPP::Message &msg)
{
	if (OpenedRoomChats.contains(msg.from().bare()))
		handleRoomChatReceivedMessage(msg);
	else
		handleContactChatReceivedMessage(msg);
}

}
