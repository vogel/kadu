/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QHash>

#include "configuration/configuration-file.h"
#include "buddies/buddy-set.h"
#include "buddies/ignored-helper.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "status/status-group.h"
#include "status/status-type.h"

#include "debug.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-protocol.h"

#include "gadu-chat-service.h"

GaduChatService::GaduChatService(GaduProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	// TODO: 0.6.6
// 	connect(protocol->socketNotifiers(), SIGNAL(ackReceived(int, uin_t, int)),
// 		this, SLOT(ackReceived(int, uin_t, int)));
}

bool GaduChatService::sendMessage(Chat chat, FormattedMessage &message)
{
	kdebugf();

	QString plain = message.toPlain();
	QList<Contact> contacts = chat.contacts().toContactList();

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	unsigned char *formats = GaduFormater::createFormats(Protocol->account(), message, formatsSize);
	bool stop = false;

	plain.replace("\r\n", "\n");
	plain.replace("\r", "\n");
	plain.replace(QChar::LineSeparator, "\n");

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = unicode2cp(plain);

	emit sendMessageFiltering(chat, data, stop);

	if (stop)
	{
		if (formats)
			delete[] formats;

		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	if (data.length() >= 2000)
	{
		if (formats)
			delete[] formats;

		MessageDialog::msg(tr("Filtered message too long (%1>=%2)").arg(data.length()).arg(2000), false, "Warning");
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return false;
	}

	uinsCount = contacts.count();

	int messageId = -1;
	if (uinsCount > 1)
	{
		UinType* uins = new UinType[uinsCount];
		unsigned int i = 0;

		foreach (const Contact &contact, contacts)
			uins[i++] = Protocol->uin(contact);
		if (formatsSize)
			messageId = gg_send_message_confer_richtext(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data(),
					formats, formatsSize);
		else
			messageId = gg_send_message_confer(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data());
		delete[] uins;
	}
	else
		foreach (const Contact &contact, contacts)
			{
				if (formatsSize)
					messageId = gg_send_message_richtext(
							Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data(),
							formats, formatsSize);
				else
					messageId = gg_send_message(
							Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data());

				break;
			}

	if (-1 == messageId)
		return false;

	if (formats)
		delete[] formats;

	Message msg(chat, Message::TypeSent, Core::instance()->myself());
	msg
		.setStatus(Message::StatusSent)
		.setContent(message.toHtml())
		.setSendDate(QDateTime::currentDateTime())
		.setReceiveDate(QDateTime::currentDateTime())
		.setId(messageId);

	UndeliveredMessages.insert(messageId, msg);
	emit messageSent(msg);

	kdebugf2();
	return true;
}

bool GaduChatService::isSystemMessage(gg_event *e)
{
	if (0 == e->event.msg.sender)
		kdebugmf(KDEBUG_INFO, "Ignored system message.\n");

	return 0 == e->event.msg.sender;
}

Buddy GaduChatService::getSender(gg_event *e)
{
	return Protocol->account().getBuddyById(QString::number(e->event.msg.sender));
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
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", sender.id(Protocol->account()).toUInt());

	return ignore;
}

BuddySet GaduChatService::getRecipients(gg_event *e)
{
	BuddySet recipients;
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
	{
		Buddy recipient = Protocol->account().getBuddyById(QString::number(e->event.msg.recipients[i]));
		recipients.insert(recipient);
	}

	return recipients;
}

QString GaduChatService::getContent(gg_event *e)
{
	QString content = cp2unicode((const char *)e->event.msg.message);

	content.replace(QLatin1String("\r\n"), QString(QChar::LineSeparator));
	content.replace(QLatin1String("\n"),   QString(QChar::LineSeparator));
	content.replace(QLatin1String("\r"),   QString(QChar::LineSeparator));

	return content;
}

bool GaduChatService::ignoreRichText(gg_event *e, Buddy sender)
{
	bool ignore = sender.isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext");

	if (ignore)
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");

	return ignore;
}

bool GaduChatService::ignoreImages(gg_event *e, Buddy sender)
{
	return sender.isAnonymous() ||
		(
			"Offline" == Protocol->status().group() ||
			(
				("Invisible" == Protocol->status().group()) &&
				!config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")
			)
		);
}

FormattedMessage GaduChatService::createFormattedMessage(gg_event *e, Buddy sender)
{
	QString content = getContent(e);

// 	bool grab = false;
// 	emit chatMsgReceived0(this, senders, mesg, time, grab);
// 	if (grab)
// 		return;

	if (ignoreRichText(e, sender))
		return GaduFormater::createMessage(Protocol->account(), sender.id(Protocol->account()).toUInt(), content, 0, 0, false);
	else
		return GaduFormater::createMessage(Protocol->account(), sender.id(Protocol->account()).toUInt(), content,
				(unsigned char *)e->event.msg.formats, e->event.msg.formats_length, !ignoreImages(e, sender));
}

void GaduChatService::handleEventMsg(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);

	if (isSystemMessage(e))
		return;

	Buddy sender = getSender(e);
	if (ignoreSender(e, sender))
		return;

	BuddySet recipients = getRecipients(e);

	BuddySet conference = recipients;
	conference += sender;
	if (IgnoredHelper::isIgnored(conference))
		return;

// 	bool ignore = false;
// 	emit rawGaduReceivedMessageFilter(Protocol->account(), senders, msg, formats, ignore); TODO: 0.6.6
// 	if (ignore)
// 		return;

	FormattedMessage message = createFormattedMessage(e, sender);
	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			sender.id(Protocol->account()).toUInt(), qPrintable(message.toPlain()));

	BuddySet chatContacts = conference;
	chatContacts.remove(Core::instance()->myself());
	
// 	QList<Contact> chatContactsList;
// 	foreach (const Buddy &buddy, chatContacts)
// 		  chatContactsList.append(buddy.prefferedContact());

	Chat chat = Protocol->findChat(chatContacts.toContactSet());

	QDateTime time = QDateTime::fromTime_t(e->event.msg.time);

	bool ignore = false;
	emit receivedMessageFilter(chat, sender, message.toPlain(), time.toTime_t(), ignore);
	if (ignore)
		return;

	Message msg(chat, Message::TypeReceived, sender);
	msg
		.setStatus(Message::StatusReceived)
		.setContent(message.toHtml())
		.setSendDate(time)
		.setReceiveDate(QDateTime::currentDateTime());
	emit messageReceived(msg);
}

void GaduChatService::handleEventAck(struct gg_event *e)
{
	kdebugf();

	int messageId = e->event.ack.seq;
	if (!UndeliveredMessages.contains(messageId))
		return;

	int uin = e->event.ack.recipient;

	Message::Status status = Message::StatusUnknown;
	switch (e->event.ack.status)
	{
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedDelivered);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedQueued);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBlocked);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBoxFull);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedUnknown);
			status = Message::StatusWontDeliver;
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, messageId, e->event.ack.status);
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

	QDateTime now = QDateTime();
	QList<int> toRemove;

	QHash<int, Message>::const_iterator message = UndeliveredMessages.constBegin();
	QHash<int, Message>::const_iterator end = UndeliveredMessages.constEnd();
	for (; message != end; message++)
	{
		if (message.value().sendDate().addSecs(MAX_DELIVERY_TIME) < now)
		{
			toRemove.append(message.key());
			UndeliveredMessages[message.key()].setStatus(Message::StatusWontDeliver);
		}

		message++;
	}

	foreach (int messageId, toRemove)
		UndeliveredMessages.remove(messageId);
}
