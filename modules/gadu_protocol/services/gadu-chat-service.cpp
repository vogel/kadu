/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/ignored-helper.h"

#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "misc/misc.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-protocol.h"

#include "gadu-chat-service.h"

GaduChatService::GaduChatService(GaduProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	connect(protocol->socketNotifiers(), SIGNAL(ackReceived(int, uin_t, int)), 
		this, SLOT(ackReceived(int, uin_t, int)));
}

bool GaduChatService::sendMessage(ContactList contacts, Message &message)
{
	kdebugf();

	message.setId(-1);
	QString plain = message.toPlain();

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	unsigned char *formats = GaduFormater::createFormats(Protocol->account(), message, formatsSize);
	bool stop = false;

	plain.replace("\r\n", "\n");
	plain.replace("\r", "\n");

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = unicode2cp(plain);

	emit sendMessageFiltering(contacts, data, stop);

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

		MessageBox::msg(tr("Filtered message too long (%1>=%2)").arg(data.length()).arg(2000), false, "Warning");
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return false;
	}

	foreach (const Contact &contact, contacts)
		if (contact.accountData(Protocol->account()))
			++uinsCount;

	if (uinsCount > 1)
	{
		UinType* uins = new UinType[uinsCount];
		unsigned int i = 0;

		foreach (const Contact &contact, contacts)
			if (contact.accountData(Protocol->account()))
				uins[i++] = Protocol->uin(contact);
		if (formatsSize)
			LastMessageId = gg_send_message_confer_richtext(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data(),
					formats, formatsSize);
		else
			LastMessageId = gg_send_message_confer(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data());
		delete[] uins;
	}
	else
		foreach (const Contact &contact, contacts)
			if (contact.accountData(Protocol->account()))
			{
				if (formatsSize)
					LastMessageId = gg_send_message_richtext(
							Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data(),
							formats, formatsSize);
				else
					LastMessageId = gg_send_message(
							Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data());

				break;
			}

	message.setId(LastMessageId);

	if (formats)
		delete[] formats;

	kdebugf2();
	return true;
}

void GaduChatService::handleEventMsg(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);

	if (0 == e->event.msg.sender) // system message, ignore
	{
		kdebugmf(KDEBUG_INFO, "Ignored system message.\n");
		return;
	}

	Contact sender(Protocol->account()->getContactById(QString::number(e->event.msg.sender)));

	if (sender.isAnonymous() &&
			config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			((e->event.msg.recipients_count == 0) || config_file.readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")))
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", Protocol->uin(sender));
		return;
	}

	ContactList recipients;
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
	{
		Contact recipient = Protocol->account()->getContactById(QString::number(e->event.msg.recipients[i]));
		recipients.append(recipient);
	}

	ContactList conference = recipients;
	conference << sender;
	if (IgnoredHelper::isIgnored(conference))
		return;

// 	bool ignore = false;
// 	emit rawGaduReceivedMessageFilter(Protocol->account(), senders, msg, formats, ignore); TODO: 0.6.6
// 	if (ignore)
// 		return;

	Message message;
	QString content = cp2unicode((const char *)e->event.msg.message);

	QDateTime time = QDateTime::fromTime_t(e->event.msg.time);

// 	bool grab = false;
// 	emit chatMsgReceived0(this, senders, mesg, time, grab);
// 	if (grab)
// 		return;

	if (sender.isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext"))
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
		message = GaduFormater::createMessage(Protocol->account(), Protocol->uin(sender), content, 0, 0, false);
	}
	else
	{
		bool receiveImages =
			!sender.isAnonymous() &&
			(
				Protocol->status().isOnline() ||
				Protocol->status().isBusy() ||
				(
					Protocol->status().isInvisible() &&
					config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")
				)
			);

		message = GaduFormater::createMessage(Protocol->account(), Protocol->uin(sender), content,
				(unsigned char *)e->event.msg.formats, e->event.msg.formats_length, receiveImages);
	}

	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			Protocol->uin(sender), qPrintable(message.toPlain()));

	bool ignore = false;
	emit receivedMessageFilter(Protocol->account(), sender, recipients, message.toPlain(), time.toTime_t(), ignore);
	if (ignore)
		return;

	emit messageReceived(Protocol->account(), sender, recipients, message.toHtml(), time.toTime_t());
}

void GaduChatService::handleEventAck(struct gg_event *e)
{
	kdebugf();

	int messageId = e->event.ack.seq;
	int uin = e->event.ack.recipient;
	if (messageId != LastMessageId)
		return;

	switch (e->event.ack.status)
	{
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedDelivered);
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedQueued);
			break;
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBlocked);
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBoxFull);
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedUnknown);
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, messageId, e->event.ack.status);
			break;
	}

	kdebugf2();
}
