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
#include "misc.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-protocol.h"

#include "gadu-chat-service.h"

GaduChatService::GaduChatService(GaduProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	connect(protocol->socketNotifiers(), SIGNAL(ackReceived(int, uin_t, int)), 
		this, SLOT(ackReceived(int, uin_t, int)));
	connect(protocol->socketNotifiers(), SIGNAL(messageReceived(int, ContactList, QString &, time_t, QByteArray &)),
		this, SLOT(messageReceivedSlot(int, ContactList, QString &, time_t, QByteArray &)));
}

bool GaduChatService::sendMessage(ContactList contacts, Message &message)
{
	kdebugf();

	message.setId(-1);
	QString plain = message.toPlain();

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	unsigned char *formats = GaduFormater::createFormats(message, formatsSize);
	bool stop = false;

	plain.replace("\r\n", "\n");
	plain.replace("\r", "\n");

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = unicode2cp(plain);

	emit sendMessageFiltering(contacts, data, stop);

	if (stop)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	if (data.length() >= 2000)
	{
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

void GaduChatService::messageReceivedSlot(int msgclass, ContactList senders, QString &msg, time_t time, QByteArray &formats)
{
/*
	najpierw sprawdzamy czy nie jest to wiadomosc systemowa (senders[0] rowne 0)
	potem sprawdzamy czy user jest na naszej liscie, jezeli nie to anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/

	if (senders[0].isAnonymous() &&
			config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			((senders.size() == 1) || config_file.readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")))
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", Protocol->uin(senders[0]));
		return;
	}

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (IgnoredHelper::isIgnored(senders))
		return;

	bool ignore = false;
	emit rawGaduReceivedMessageFilter(Protocol->account(), senders, msg, formats, ignore);
	if (ignore)
		return;

	const char* msg_c = msg.toAscii().constData();

	Message message;
	QString content = cp2unicode(msg_c);

	QDateTime datetime;
	datetime.setTime_t(time);

// 	bool grab = false;
// 	emit chatMsgReceived0(this, senders, mesg, time, grab);
// 	if (grab)
// 		return;

	// wiadomosci systemowe maja senders[0] = 0
	// FIX ME!!!
	if (Protocol->uin(senders[0]) == 0)
	{
		if (msgclass <= config_file.readNumEntry("General", "SystemMsgIndex", 0))
		{
			kdebugm(KDEBUG_INFO, "Already had this message, ignoring\n");
			return;
		}

		config_file.writeEntry("General", "SystemMsgIndex", msgclass);
		kdebugm(KDEBUG_INFO, "System message index %d\n", msgclass);

		return;
	}

	if (senders[0].isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext"))
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
		message = GaduFormater::createMessage(Protocol->account(), Protocol->uin(senders[0]), content, 0, 0, false);
	}
	else
	{
		bool receiveImages =
			!senders[0].isAnonymous() &&
			(
				Protocol->status().isOnline() ||
				Protocol->status().isBusy() ||
				(
					Protocol->status().isInvisible() &&
					config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")
				)
			);

		message = GaduFormater::createMessage(Protocol->account(), Protocol->uin(senders[0]), content,
				(unsigned char *)formats.data(), formats.size(), receiveImages);
	}

	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			Protocol->uin(senders[0]), qPrintable(message.toPlain()));

	emit receivedMessageFilter(Protocol->account(), senders, message.toPlain(), time, ignore);
	if (ignore)
		return;

	emit messageReceived(Protocol->account(), senders, message.toHtml(), time);
}

void GaduChatService::ackReceived(int messageId, uin_t uin, int status)
{
	kdebugf();

	if (messageId != LastMessageId)
		return;

	switch (status)
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
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, messageId, status);
			break;
	}
	kdebugf2();
}
