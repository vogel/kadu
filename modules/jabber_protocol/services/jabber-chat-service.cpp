/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"

#include "html_document.h"
#include "debug.h"

#include "../jabber-protocol.h"

#include "jabber-chat-service.h"

JabberChatService::JabberChatService(JabberProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	connect(protocol->client(), SIGNAL(messageReceived(const XMPP::Message &)),
		this, SLOT(clientMessageReceived(const XMPP::Message &)));
}

bool JabberChatService::sendMessage(const Chat &chat, FormattedMessage &formattedMessage, bool silent)
{
	kdebugf();
	ContactSet contacts = chat.contacts();
        // TODO send to more users
	if (contacts.count() > 1 || contacts.count() == 0)
		return false;

	//QString cleanmsg = toPlainText(mesg);
	QString plain = formattedMessage.toPlain();
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

	msg.setType("chat");
	msg.setBody(plain);
	msg.setTimeStamp(QDateTime::currentDateTime());
	//msg.setFrom(jabberID);
	Protocol->client()->sendMessage(msg);

	if (!silent)
	{
		HtmlDocument::escapeText(plain);

		Message message = Message::create();
		message.setMessageChat(chat);
		message.setType(Message::TypeSent);
		message.setMessageSender(Protocol->account().accountContact());
		message.setContent(Qt::escape(formattedMessage.toPlain()));
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		emit messageSent(message);
	}

	kdebugf2();
	return true;
}

void JabberChatService::clientMessageReceived(const XMPP::Message &msg)
{
	kdebugf();

	// skip empty messages, but not if the message contains a data form
	if(msg.body().isEmpty() && msg.urlList().isEmpty() && msg.invite().isEmpty() && !msg.containsEvents() && msg.chatState() == XMPP::StateNone
		&& msg.subject().isEmpty() && msg.rosterExchangeItems().isEmpty() && msg.mucInvites().isEmpty() &&  msg.getForm().fields().empty())
		return;

	// TODO zapobiega otwieraniu okienka z pusta wiadomoscia
	if ((XMPP::StateNone != msg.chatState()) && msg.body().isEmpty())
		return;

	// skip messages with type error == Cancel (fixes mantis #1642)
	if (msg.type () == "error")
		return;

	// dalej obslugujemy juz tylko wiadomosci
	if (msg.containsEvents() && msg.body().isEmpty())
		return;

	Contact contact = ContactManager::instance()->byId(Protocol->account(), msg.from().bare(), ActionCreateAndAdd);
	ContactSet contacts(contact);

	Chat chat = ChatManager::instance()->findChat(contacts);
	bool ignore = false;

	time_t msgtime = msg.timeStamp().toTime_t();
	QByteArray body = msg.body().toUtf8();
	emit filterRawIncomingMessage(chat, contact, body, ignore);

	FormattedMessage formattedMessage(QString::fromUtf8(body));

	QString plain = formattedMessage.toPlain();

	emit filterIncomingMessage(chat, contact, plain, msgtime, ignore);
	if (ignore)
		return;

	HtmlDocument::escapeText(plain);

	Message message = Message::create();
	message.setMessageChat(chat);
	message.setType(Message::TypeReceived);
	message.setMessageSender(contact);
	message.setContent(plain);
	message.setSendDate(msg.timeStamp());
	message.setReceiveDate(QDateTime::currentDateTime());

	emit messageReceived(message);

	kdebugf2();
}
