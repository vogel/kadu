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

#include "buddies/buddy-set.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "message/message.h"
#include "misc/misc.h"

#include "debug.h"
#include "html_document.h"

#include "../jabber-protocol.h"

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

void JabberChatService::clientDestroyed()
{
	XmppClient = 0;
}

void JabberChatService::setClient(Client *xmppClient)
{
	if (XmppClient)
		disconnect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));

	XmppClient = xmppClient;

	if (XmppClient)
		connect(XmppClient, SIGNAL(destroyed()), this, SLOT(clientDestroyed()));
}

bool JabberChatService::sendMessage(const Chat &chat, const QString &message, bool silent)
{
	if (!XmppClient)
		return false;

	kdebugf();
	ContactSet contacts = chat.contacts();
	// TODO send to more users
	if (contacts.count() > 1 || contacts.count() == 0)
		return false;

	QTextDocument document;
	document.setHtml(message);

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
		message.setContent(plain);
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		emit messageSent(message);
	}

	kdebugf2();
	return true;
}

void JabberChatService::handleReceivedMessage(const XMPP::Message &msg)
{
	kdebugf();

	// skip empty messages
	if (msg.body().isEmpty())
		return;

	// skip messages with type error == Cancel (fixes bug #1642)
	if (msg.type() == "error")
		return;

	Contact contact = ContactManager::instance()->byId(account(), msg.from().bare(), ActionCreateAndAdd);
	ContactSet contacts(contact);

	Chat chat = ChatManager::instance()->findChat(contacts);
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

	kdebugf2();
}

}
