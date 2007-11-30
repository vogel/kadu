/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qsocketnotifier.h>

#include "libgadu.h"

#include "config_file.h"
#include "debug.h"
#include "gadu-private.h"
#include "ignore.h"
#include "misc.h"
#include "userlist.h"

SocketNotifiers::SocketNotifiers(int fd, QObject *parent, const char *name) : QObject(parent, name), Fd(fd), Snr(0), Snw(0)
{
	kdebugf();
	kdebugf2();
}

SocketNotifiers::~SocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::start()
{
	kdebugf();
	createSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::stop()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void SocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	Snr = new QSocketNotifier(Fd, QSocketNotifier::Read, this, "read_socket_notifier");
	connect(Snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	Snw = new QSocketNotifier(Fd, QSocketNotifier::Write, this, "write_socket_notifier");
	connect(Snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

	kdebugf2();
}

void SocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (Snr)
	{
		Snr->setEnabled(false);
		Snr->deleteLater();
		Snr = NULL;
	}

	if (Snw)
	{
		Snw->setEnabled(false);
		Snw->deleteLater();
		Snw = NULL;
	}

	kdebugf2();
}

void SocketNotifiers::recreateSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();
	createSocketNotifiers();

	kdebugf2();
}

/* PubdirSocketNotifiers */

PubdirSocketNotifiers::PubdirSocketNotifiers(struct gg_http *h, QObject *parent, const char *name)
	: SocketNotifiers(h->fd, parent, name), H(h)
{
	kdebugf();
	kdebugf2();
}

PubdirSocketNotifiers::~PubdirSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void PubdirSocketNotifiers::dataReceived()
{
	kdebugf();

	if (H->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void PubdirSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void PubdirSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_pubdir_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit done(false, H);
		gg_pubdir_free(H);
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{
		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers\n");
			recreateSocketNotifiers();

			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");
			deleteSocketNotifiers();
			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();

			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success!\n");
				emit done(true, H);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");
				emit done(false, H);
			}
			gg_pubdir_free(H);
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
	}
	kdebugf2();
}

/* TokenSocketNotifier */

TokenSocketNotifiers::TokenSocketNotifiers(QObject *parent, const char *name)
	: SocketNotifiers(0, parent, name), H(0)
{
	kdebugf();
	kdebugf2();
}

TokenSocketNotifiers::~TokenSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void TokenSocketNotifiers::start()
{
	kdebugf();

	if (!(H = gg_token(1)))
	{
		emit tokenError();
		return;
	}

	Fd = H->fd;
	createSocketNotifiers();
	kdebugf2();
}

void TokenSocketNotifiers::dataReceived()
{
	kdebugf();

	if (H->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void TokenSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void TokenSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_token_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit tokenError();
		gg_token_free(H);
		H = NULL;
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{

		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
			break;

		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			emit tokenError();
			gg_token_free(H);
			H = NULL;
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();
			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success\n");

				struct gg_token *t = (struct gg_token *)H->data;
				QString tokenId = cp2unicode((unsigned char *)t->tokenid);

				//nie optymalizowac!!!
				QByteArray buf(H->body_size);
				for (unsigned int i = 0; i < H->body_size; ++i)
					buf[i] = H->body[i];

				QPixmap tokenImage(buf);

				emit gotToken(tokenId, tokenImage);
			}

			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
				emit tokenError();
			}

			gg_token_free(H);
			H = NULL;
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);
	}

	kdebugf2();
}

/* GaduSocketNotifiers */

GaduSocketNotifiers::GaduSocketNotifiers(QObject *parent, const char *name) :
	SocketNotifiers(0, parent, name), Sess(0), socketEventCalls(0)
{
	kdebugf();
	kdebugf2();
}

GaduSocketNotifiers::~GaduSocketNotifiers()
{
}

void GaduSocketNotifiers::setSession(gg_session *sess)
{
	Sess = sess;
	Fd = Sess->fd;
}

void GaduSocketNotifiers::checkWrite()
{
	kdebugf();
	//kiedy¶ tu siê sypa³o, ale b³±d zosta³ naprawiony
	//mimo to niech kdebugi zostan± w razie gdyby ten b³±d kiedy¶ powróci³
	if (Sess == NULL)
	{
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
		printBacktrace("GaduSocketNotifiers::checkWrite(): Sess==null");
	}
	if (Sess->check & GG_CHECK_WRITE)
	{
		if (Snw == NULL)
		{
			kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");
			printBacktrace("GaduSocketNotifiers::checkWrite(): Snw==null");
		}
		Snw->setEnabled(true);
	}
	kdebugf2();
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	if (Sess->check & GG_CHECK_READ)
		socketEvent();

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	if (Sess==NULL)
	{
		kdebugm(KDEBUG_PANIC, "Sess == NULL !!\n");
		printBacktrace("GaduSocketNotifiers::dataSent(): Sess==null");
	}
	if (Snw==NULL)
	{
		kdebugm(KDEBUG_PANIC, "Snw == NULL !!\n");
		printBacktrace("GaduSocketNotifiers::dataSent(): Snw==null");
	}

	Snw->setEnabled(false);
	if (Sess->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void GaduSocketNotifiers::socketEvent()
{
	kdebugf();

	++socketEventCalls;
	if (socketEventCalls > 1)
		kdebugm(KDEBUG_WARNING, "************* GaduSocketNotifiers::socketEvent(): Recursive socketEvent calls detected!\n");

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
			UserListElements users(userlist->byID("Gadu", QString::number(e->event.msg.sender)));

			if (e->event.msg.msgclass == GG_CLASS_CTCP)
			{
				if (config_file.readBoolEntry("Network", "AllowDCC") && !IgnoredManager::isIgnored(users) && !users[0].isAnonymous())
					emit dccConnectionReceived(users[0]);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);
				if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT)
					for (int i = 0; i < e->event.msg.recipients_count; ++i)
						users.append(userlist->byID("Gadu", QString::number(e->event.msg.recipients[i])));
				QCString msg((char*)e->event.msg.message);
				QByteArray formats;
				formats.duplicate((const char*)e->event.msg.formats, e->event.msg.formats_length);
				emit messageReceived(e->event.msg.msgclass, users, msg,
					e->event.msg.time, formats);
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

void GaduSocketNotifiers::connectionFailed(int failure)
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

Protocol *GaduProtocolManager::newInstance(const QString &id)
{
	return new GaduProtocol(id, protocols_manager, "gadu_protocol_");
}
