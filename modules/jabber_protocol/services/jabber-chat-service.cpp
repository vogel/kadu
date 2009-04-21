/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/ignored-helper.h"

#include "chat/chat.h"
#include "config_file.h"
#include "debug.h"
#include "../jabber_protocol.h"
#include "message_box.h"
#include "misc/misc.h"

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
	connect(protocol->client(), SIGNAL (messageReceived ( const XMPP::Message & ) ),
		this, SLOT (clientMessageReceived(const XMPP::Message & )));
}

bool JabberChatService::sendMessage(Chat *chat, Message &message)
{
	kdebugf();
	ContactSet contacts = chat->contacts();
        // TODO send to more users
	Contact contact = (*contacts.begin());
	//QString cleanmsg = toPlainText(mesg);
	QString plain = message.toPlain();
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
	Contact contact = Protocol->account()->getContactById(msg.from().bare());
	ContactSet contacts = ContactSet(contact);
	time_t msgtime = msg.timeStamp().toTime_t();
	Message message(msg.body());

	bool ignore = false;
	Chat *chat = Protocol->findChat(contacts);
	emit receivedMessageFilter(chat, contact, message.toPlain(), msgtime, ignore);
	if (ignore)
		return;

	emit messageReceived(chat, contact, message.toHtml());
	kdebugf2();
}

