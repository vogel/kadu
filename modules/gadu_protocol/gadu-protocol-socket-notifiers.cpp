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

GaduProtocolSocketNotifiers::GaduProtocolSocketNotifiers(Account *account, QObject *parent) :
		CurrentAccount(account), GaduSocketNotifiers(0, parent), Sess(0), socketEventCalls(0)
{
	kdebugf();
	kdebugf2();
}

GaduProtocolSocketNotifiers::~GaduProtocolSocketNotifiers()
{
}

void GaduProtocolSocketNotifiers::setSession(gg_session *sess)
{
	Sess = sess;
	Fd = Sess->fd;
}

void GaduProtocolSocketNotifiers::checkWrite()
{
	kdebugf();
	//kiedy� tu si� sypa�o, ale b��d zosta� naprawiony
	//mimo to niech kdebugi zostan� w razie gdyby ten b��d kiedy� powr�ci�
	if (Sess == NULL)
	{
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
		printBacktrace("GaduProtocolSocketNotifiers::checkWrite(): Sess==null");
	}
	if (Sess->check & GG_CHECK_WRITE)
	{
		if (Snw == NULL)
		{
			kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");
			printBacktrace("GaduProtocolSocketNotifiers::checkWrite(): Snw==null");
		}
		Snw->setEnabled(true);
	}
	kdebugf2();
}

void GaduProtocolSocketNotifiers::dataReceived()
{
	kdebugf();

	Snr->setEnabled(false);

	if (Sess->check & GG_CHECK_READ)
		socketEvent();

	if(Snr) Snr->setEnabled(true);
	

	kdebugf2();
}

void GaduProtocolSocketNotifiers::dataSent()
{
	kdebugf();

	if (Sess==NULL)
	{
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
		printBacktrace("GaduProtocolSocketNotifiers::dataSent(): Sess==null");
	}
	if (Snw==NULL)
	{
		kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");
		printBacktrace("GaduProtocolSocketNotifiers::dataSent(): Snw==null");
	}

	Snw->setEnabled(false);
	if (Sess->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
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
		emit error(ConnectionUnknow);
//		gg_free_event(e);//nulla nie zwalniamy, bo i po co?
		--socketEventCalls;
		return;
	}

	if (Sess->state == GG_STATE_CONNECTING_HUB || Sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");

		recreateSocketNotifiers();
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
						!senders[0].isAnonymous())
					emit dccConnectionReceived(senders[0]);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);
				if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT)
					for (int i = 0; i < e->event.msg.recipients_count; ++i)
						senders.append(CurrentAccount->getContactById(QString::number(e->event.msg.recipients[i])));
				QString msg((char*)e->event.msg.message);
				QByteArray formats;
				formats.duplicate((const char*)e->event.msg.formats, e->event.msg.formats_length);
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
	GaduError err;

	switch (failure)
	{
		case GG_FAILURE_RESOLVING:	err = ConnectionServerNotFound; break;
		case GG_FAILURE_CONNECTING:	err = ConnectionCannotConnect; break;
		case GG_FAILURE_NEED_EMAIL:	err = ConnectionNeedEmail; break;
		case GG_FAILURE_INVALID:	err = ConnectionInvalidData; break;
		case GG_FAILURE_READING:	err = ConnectionCannotRead; break;
		case GG_FAILURE_WRITING:	err = ConnectionCannotWrite; break;
		case GG_FAILURE_PASSWORD:	err = ConnectionIncorrectPassword; break;
		case GG_FAILURE_TLS:		err = ConnectionTlsError; break;
		case GG_FAILURE_INTRUDER:	err = ConnectionIntruderError; break;
		case GG_FAILURE_UNAVAILABLE:	err = ConnectionUnavailableError; break;
		default:
			kdebugm(KDEBUG_ERROR, "ERROR: unhandled/unknown connection error! %d\n", failure);
			err=ConnectionUnknow;
	}

	emit error(err);
	kdebugf2();
}
