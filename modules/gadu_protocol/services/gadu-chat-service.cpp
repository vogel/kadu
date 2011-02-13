/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QHash>
#include <QtCore/QScopedArrayPointer>

#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "status/status-group.h"
#include "status/status-type.h"

#include "debug.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-chat-service.h"

GaduChatService::GaduChatService(GaduProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	// TODO
// 	connect(protocol->socketNotifiers(), SIGNAL(ackReceived(int, uin_t, int)),
// 		this, SLOT(ackReceived(int, uin_t, int)));
}

bool GaduChatService::sendMessage(const Chat &chat, FormattedMessage &message, bool silent)
{
	kdebugf();

	QString plain = message.toPlain();
	QList<Contact> contacts = chat.contacts().toContactList();

	if (plain.isEmpty()) // for image sending
		message.prepend(FormattedMessagePart(" ", false, false, false, QColor(0, 0, 0)));

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	QScopedArrayPointer<unsigned char> formats(GaduFormatter::createFormats(Protocol->account(), message, formatsSize));
	bool stop = false;

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = plain.toUtf8();

	emit filterRawOutgoingMessage(chat, data, stop);
	plain = QString::fromUtf8(data);
	emit filterOutgoingMessage(chat, plain, stop);

	if (stop)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	if (data.length() >= 2000)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Filtered message too long (%1>=%2)").arg(data.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return false;
	}

	uinsCount = contacts.count();

	int messageId = -1;
	if (uinsCount > 1)
	{
		QScopedArrayPointer<UinType> uins(new UinType[uinsCount]);
		unsigned int i = 0;

		foreach (const Contact &contact, contacts)
			uins[i++] = Protocol->uin(contact);

		if (formatsSize)
			messageId = gg_send_message_confer_richtext(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins.data(), (unsigned char *)data.data(),
					formats.data(), formatsSize);
		else
			messageId = gg_send_message_confer(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins.data(), (unsigned char *)data.data());
	}
	else if (uinsCount == 1)
	{
		if (formatsSize)
			messageId = gg_send_message_richtext(
					Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contacts.at(0)), (unsigned char *)data.data(),
					formats.data(), formatsSize);
		else
			messageId = gg_send_message(
					Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contacts.at(0)), (unsigned char *)data.data());
	}

	if (-1 == messageId)
		return false;


	if (!silent)
	{
		Message msg = Message::create();
		msg.setMessageChat(chat);
		msg.setType(Message::TypeSent);
		msg.setMessageSender(Protocol->account().accountContact());
		msg.setStatus(Message::StatusSent);
		msg.setContent(message.toHtml());
		msg.setSendDate(QDateTime::currentDateTime());
		msg.setReceiveDate(QDateTime::currentDateTime());
		msg.setId(messageId);

		UndeliveredMessages.insert(messageId, msg);
		emit messageSent(msg);
	}

	kdebugf2();
	return true;
}

bool GaduChatService::isSystemMessage(gg_event *e)
{
	if (0 == e->event.msg.sender)
	{
		kdebugmf(KDEBUG_INFO, "Ignored system message.\n");
	}

	return 0 == e->event.msg.sender;
}

Contact GaduChatService::getSender(gg_event *e)
{
	return ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.msg.sender), ActionCreateAndAdd);
}

bool GaduChatService::ignoreSender(gg_event *e, Buddy sender)
{
	bool ignore =
			sender.isAnonymous() &&
			config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			(
				(e->event.msg.recipients_count == 0) ||
				config_file.readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")
			);

	if (ignore)
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %u is ignored\n", sender.id(Protocol->account()).toUInt());
	}

	return ignore;
}

ContactSet GaduChatService::getRecipients(gg_event *e)
{
	ContactSet recipients;
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
		recipients.insert(ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.msg.recipients[i]), ActionCreateAndAdd));

	return recipients;
}

QByteArray GaduChatService::getContent(gg_event *e)
{
	return QByteArray((const char *)e->event.msg.message);
}

bool GaduChatService::ignoreRichText(Contact sender)
{
	bool ignore = sender.ownerBuddy().isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext");

	if (ignore)
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
	}

	return ignore;
}

bool GaduChatService::ignoreImages(Contact sender)
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());

	return sender.ownerBuddy().isAnonymous() ||
		(
			"Offline" == Protocol->status().group() ||
			(
				("Invisible" == Protocol->status().group()) &&
				!gaduAccountDetails->receiveImagesDuringInvisibility()
			)
		);
}

FormattedMessage GaduChatService::createFormattedMessage(struct gg_event *e, const QByteArray &content, Contact sender)
{
	if (ignoreRichText(sender))
		return GaduFormatter::createMessage(Protocol->account(), sender.id().toUInt(), QString::fromUtf8(content), 0, 0, false);
	else
		return GaduFormatter::createMessage(Protocol->account(), sender.id().toUInt(), QString::fromUtf8(content),
				(unsigned char *)e->event.msg.formats, e->event.msg.formats_length, !ignoreImages(sender));
}

void GaduChatService::handleMsg(Contact sender, ContactSet recipients, Message::Type type, gg_event *e)
{
	ContactSet conference = recipients;
	conference += sender;

	ContactSet chatContacts = conference;
	chatContacts.remove(Protocol->account().accountContact());

	Chat chat = ChatManager::instance()->findChat(chatContacts);
	if (chat.isIgnoreAllMessages())
		return;

	QByteArray content = getContent(e);
	QDateTime time = QDateTime::fromTime_t(e->event.msg.time);

	bool ignore = false;
	emit filterRawIncomingMessage(chat, sender, content, ignore);

	FormattedMessage message = createFormattedMessage(e, content, sender);
	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %u saying \"%s\"\n",
			sender.id().toUInt(), qPrintable(message.toPlain()));

	QString messageString = message.toPlain();
	emit filterIncomingMessage(chat, sender, messageString, time.toTime_t(), ignore);
	if (ignore)
		return;

	Message msg = Message::create();
	msg.setMessageChat(chat);
	msg.setType(type);
	msg.setMessageSender(sender);
	msg.setStatus(Message::TypeReceived == type ? Message::StatusReceived : Message::StatusSent);
	msg.setContent(message.toHtml());
	msg.setSendDate(time);
	msg.setReceiveDate(QDateTime::currentDateTime());

	if (Message::TypeReceived == type)
		emit messageReceived(msg);
	else
		emit messageSent(msg);
}

void GaduChatService::handleEventMsg(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);

	if (isSystemMessage(e))
		return;

	Contact sender = getSender(e);
	if (ignoreSender(e, sender.ownerBuddy()))
		return;

	ContactSet recipients = getRecipients(e);

	handleMsg(sender, recipients, Message::TypeReceived, e);
}

#ifdef GADU_HAVE_MULTILOGON
void GaduChatService::handleEventMultilogonMsg(gg_event *e)
{
	// warning: this may be not intuitive code

	// we are sender
	Contact sender = Protocol->account().accountContact();

	// e.sender + e.recipeints are real recipients
	ContactSet recipients = getRecipients(e);
	recipients.insert(getSender(e));

	handleMsg(sender, recipients, Message::TypeSent, e);
}
#endif

void GaduChatService::handleEventAck(struct gg_event *e)
{
	kdebugf();

	int messageId = e->event.ack.seq;
	if (!UndeliveredMessages.contains(messageId))
		return;

	UinType uin = e->event.ack.recipient;
	Q_UNUSED(uin) // only in debug mode

	Message::Status status = Message::StatusUnknown;
	switch (e->event.ack.status)
	{
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %u, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedDelivered);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %u, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedQueued);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %u, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBlocked);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %u, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBoxFull);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %u, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedUnknown);
			status = Message::StatusWontDeliver;
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %u, seq: %d, status:%d)\n", uin, messageId, e->event.ack.status);
			break;
	}

	UndeliveredMessages[messageId].setStatus(status);
	UndeliveredMessages.remove(messageId);

	removeTimeoutUndeliveredMessages();

	kdebugf2();
}

void GaduChatService::removeTimeoutUndeliveredMessages()
{
// TODO: move to const or something
	#define MAX_DELIVERY_TIME 60

	QDateTime now;
	QList<int> toRemove;

	QHash<int, Message>::iterator message = UndeliveredMessages.begin();
	QHash<int, Message>::iterator end = UndeliveredMessages.end();
	for (; message != end; ++message)
	{
		if (message.value().sendDate().addSecs(MAX_DELIVERY_TIME) < now)
		{
			toRemove.append(message.key());
			message.value().setStatus(Message::StatusWontDeliver);
		}
	}

	foreach (int messageId, toRemove)
		UndeliveredMessages.remove(messageId);
}
