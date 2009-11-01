/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/chat.h"
#include "chat/message/message.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "buddies/ignored-helper.h"
#include "gui/windows/message-box.h"
#include "misc/misc.h"

#include "html_document.h"
#include "debug.h"

#include "../jabber-protocol.h"

#include "jabber-chat-service.h"

JabberChatService::JabberChatService(JabberProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	//TODO move here some functions from jabber_protocol

	//connect(protocol, SIGNAL(ackReceived(int, uin_t, int)),
	//	this, SLOT(ackReceived(int, uin_t, int)));
// 	connect(protocol, SIGNAL(sendMessageFiltering(Chat *, QByteArray &, bool &)),
// 		this, SIGNAL(sendMessageFiltering(Chat *, QByteArray &, bool &)));
// 	connect(protocol, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)),
//     		this, SIGNAL(messageStatusChanged(int , ChatService::MessageStatus)));
// 	connect(protocol, SIGNAL(receivedMessageFilter(Chat *, Contact, const QString &, time_t , bool &)),
// 		this, SIGNAL(receivedMessageFilter(Chat *, Contact, const QString &, time_t, bool &)));
	connect(protocol->client(), SIGNAL(messageReceived(const XMPP::Message &)),
		this, SLOT(clientMessageReceived(const XMPP::Message &)));
}

bool JabberChatService::sendMessage(Chat *chat, FormattedMessage &formattedMessage)
{
	kdebugf();
	ContactSet contacts = chat->contacts();
        // TODO send to more users
	Contact contact = (*contacts.begin());
	//QString cleanmsg = toPlainText(mesg);
	QString plain = formattedMessage.toPlain();
	const XMPP::Jid jus = contact.id(Protocol->account());
	XMPP::Message msg = XMPP::Message(jus);

	bool stop = false;
	///plain na QByteArray
	QByteArray data = unicode2cp(plain);

	emit sendMessageFiltering(chat, data, stop);
	if (stop)
	{
	    // TODO: implement formats
	    kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
	    return false;
	}

	msg.setType("chat");
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());
	//msg.setFrom(jabberID);
	Protocol->client()->sendMessage(msg);

	HtmlDocument::escapeText(plain);

	Message message(chat, Message::TypeSent, Core::instance()->myself());
	message
		.setContent(plain)
		.setSendDate(QDateTime::currentDateTime())
		.setReceiveDate(QDateTime::currentDateTime());

	emit messageSent(message);

	kdebugf2();
	return true;
}

void JabberChatService::clientMessageReceived(const XMPP::Message &msg)
{
	kdebugf();

	// TODO support for events
	if (msg.containsEvents() && msg.body().isEmpty())
		return;

	// TODO zapobiega otwieraniu okienka z pusta wiadomoscia
	if ((XMPP::StateNone != msg.chatState()) && msg.body().isEmpty())
		return;

	// dalej obslugujemy juz tylko wiadomosci
	if (msg.body().isEmpty())
		return;

	// TODO - zaimplementowac to samo w ContactList
	Contact contact = Protocol->account().getContactById(msg.from().bare());
	ContactSet contacts = ContactSet(contact);
	time_t msgtime = msg.timeStamp().toTime_t();
	FormattedMessage formattedMessage(msg.body());

	QString plain = formattedMessage.toPlain();

	bool ignore = false;
	Chat *chat = Protocol->findChat(contacts);
	emit receivedMessageFilter(chat, contact, plain, msgtime, ignore);
	if (ignore)
		return;

	HtmlDocument::escapeText(plain);

	Message message(chat, Message::TypeReceived, contact);
	message
		.setContent(plain)
		.setSendDate(msg.timeStamp())
		.setReceiveDate(QDateTime::currentDateTime());

	emit messageReceived(message);

	kdebugf2();
}

