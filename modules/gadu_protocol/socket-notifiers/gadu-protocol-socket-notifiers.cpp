/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>

#include <libgadu.h>

#include "accounts/account.h"

#include "contacts/ignored-helper.h"

#include "config_file.h"
#include "debug.h"
#include "misc.h"

#include "gadu.h"

#include "gadu-protocol-socket-notifiers.h"

void GaduProtocolSocketNotifiers::watchFor(gg_session *sess)
{
	Sess = sess;
	GaduSocketNotifiers::watchFor(Sess ? Sess->fd : 0);
}

bool GaduProtocolSocketNotifiers::checkRead()
{
	return Sess->check & GG_CHECK_READ;
}

bool GaduProtocolSocketNotifiers::checkWrite()
{
	return Sess->check & GG_CHECK_WRITE;
}

void GaduProtocolSocketNotifiers::socketEvent()
{
	kdebugf();

	++socketEventCalls;
	if (socketEventCalls > 1)
		kdebugm(KDEBUG_WARNING, "************* GaduProtocolSocketNotifiers::socketEvent(): Recursive socketEvent calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(Sess)))
	{
		emit error(GaduProtocol::ConnectionUnknow);
//		gg_free_event(e);//nulla nie zwalniamy, bo i po co?
		--socketEventCalls;
		return;
	}

	if (Sess->state == GG_STATE_CONNECTING_HUB || Sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
		watchFor(Sess); // maybe fd has changed
	}

	switch (Sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Waiting for hash key\n");
			break;
		case GG_STATE_READING_REPLY:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Sending key\n");
			break;
		case GG_STATE_CONNECTED:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "connected\n");
			break;
		case GG_STATE_IDLE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "idle!\n");
			break;
		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "state==error! error=%d\n", Sess->error);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown state! state=%d\n", Sess->state);
			break;
	}

	switch (e->type)
	{
		case GG_EVENT_MSG:
		{
			ContactList senders(CurrentAccount->getContactById(QString::number(e->event.msg.sender)));

			// TODO: 0.6.6
			if (e->event.msg.recipients_count)
				break;

			if (e->event.msg.msgclass == GG_CLASS_CTCP)
			{
				if (config_file.readBoolEntry("Network", "AllowDCC") &&
						!IgnoredHelper::isIgnored(senders) &&
						!senders[0].isAnonymous() &&
						e->event.msg.message[0] == '\002')
					emit dccConnectionRequestReceived(senders[0]);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);
				if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT)
					for (int i = 0; i < e->event.msg.recipients_count; ++i)
						senders.append(CurrentAccount->getContactById(QString::number(e->event.msg.recipients[i])));
				QString msg((char*)e->event.msg.message);
				QByteArray formats((const char*)e->event.msg.formats, e->event.msg.formats_length);
				emit messageReceived(e->event.msg.msgclass, senders, msg, e->event.msg.time, formats);
			}
			break;
		}

		case GG_EVENT_IMAGE_REQUEST:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image request received\n");
			emit imageRequestReceived(
				e->event.image_request.sender,
				e->event.image_request.size,
				e->event.image_request.crc32);
			break;

		case GG_EVENT_IMAGE_REPLY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image reply received\n");
			emit imageReceived(
				e->event.image_reply.sender,
				e->event.image_reply.size,
				e->event.image_reply.crc32,
				e->event.image_reply.filename,
				e->event.image_reply.image);
			break;

		case GG_EVENT_STATUS60:
		case GG_EVENT_STATUS:
			emit userStatusChanged(e);
			break;

		case GG_EVENT_ACK:
			emit ackReceived(e->event.ack.seq, e->event.ack.recipient, e->event.ack.status);
			break;

		case GG_EVENT_NOTIFY60:
			emit userlistReceived(e);
			break;

		case GG_EVENT_PUBDIR50_SEARCH_REPLY:
		case GG_EVENT_PUBDIR50_READ:
		case GG_EVENT_PUBDIR50_WRITE:
			emit pubdirReplyReceived(e->event.pubdir50);
			break;

		case GG_EVENT_USERLIST:
			emit userlistReplyReceived(e->event.userlist.type, e->event.userlist.reply);
			break;

		case GG_EVENT_CONN_SUCCESS:
			emit connected();
			break;

		case GG_EVENT_CONN_FAILED:
			connectionFailed(e->event.failure);
			break;

		case GG_EVENT_DISCONNECT:
			emit serverDisconnected();
			break;

		case GG_EVENT_NONE:
			kdebugm(KDEBUG_NETWORK, "GG_EVENT_NONE\n");
			break;

		case GG_EVENT_DCC7_NEW:
			emit dcc7New(e->event.dcc7_new);
			break;

		case GG_EVENT_DCC7_ACCEPT:
			emit dcc7Accepted(e->event.dcc7_accept.dcc7);
			break;

		case GG_EVENT_DCC7_REJECT:
			emit dcc7Rejected(e->event.dcc7_reject.dcc7);
			break;

		case GG_EVENT_XML_EVENT:
			kdebugm(KDEBUG_NETWORK, "GG_EVENT_XML_EVENT\n");
			break;
	}

	gg_free_event(e);
	--socketEventCalls;
	kdebugf2();
}

void GaduProtocolSocketNotifiers::connectionFailed(int failure)
{
	kdebugf();
	GaduProtocol::GaduError err;

	switch (failure)
	{
		case GG_FAILURE_RESOLVING:	err = GaduProtocol::ConnectionServerNotFound; break;
		case GG_FAILURE_CONNECTING:	err = GaduProtocol::ConnectionCannotConnect; break;
		case GG_FAILURE_NEED_EMAIL:	err = GaduProtocol::ConnectionNeedEmail; break;
		case GG_FAILURE_INVALID:	err = GaduProtocol::ConnectionInvalidData; break;
		case GG_FAILURE_READING:	err = GaduProtocol::ConnectionCannotRead; break;
		case GG_FAILURE_WRITING:	err = GaduProtocol::ConnectionCannotWrite; break;
		case GG_FAILURE_PASSWORD:	err = GaduProtocol::ConnectionIncorrectPassword; break;
		case GG_FAILURE_TLS:		err = GaduProtocol::ConnectionTlsError; break;
		case GG_FAILURE_INTRUDER:	err = GaduProtocol::ConnectionIntruderError; break;
		case GG_FAILURE_UNAVAILABLE:	err = GaduProtocol::ConnectionUnavailableError; break;
		default:
			kdebugm(KDEBUG_ERROR, "ERROR: unhandled/unknown connection error! %d\n", failure);
			err = GaduProtocol::ConnectionUnknow;
	}

	emit error(err);
	kdebugf2();
}
