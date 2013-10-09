/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QSocketNotifier>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <libgadu.h>

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include "gadu-protocol-socket-notifiers.h"

GaduProtocolSocketNotifiers::GaduProtocolSocketNotifiers(Account account, GaduProtocol *protocol) :
		GaduSocketNotifiers(protocol), CurrentAccount(account), CurrentProtocol(protocol), Sess(0)
{
}

void GaduProtocolSocketNotifiers::watchFor(gg_session *sess)
{
	Sess = sess;
	GaduSocketNotifiers::watchFor(Sess ? Sess->fd : -1);
}

bool GaduProtocolSocketNotifiers::checkRead()
{
	return Sess->check & GG_CHECK_READ;
}

bool GaduProtocolSocketNotifiers::checkWrite()
{
	return Sess->check & GG_CHECK_WRITE;
}

void GaduProtocolSocketNotifiers::dumpConnectionState()
{
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
}

void GaduProtocolSocketNotifiers::handleEventMultilogonInfo(gg_event* e)
{
	CurrentProtocol->CurrentMultilogonService->handleEventMultilogonInfo(e);
}

void GaduProtocolSocketNotifiers::handleEventNotify(struct gg_event *e)
{
	struct gg_notify_reply *notify = (GG_EVENT_NOTIFY_DESCR == e->type)
			? e->event.notify_descr.notify
			: e->event.notify;

	while (notify->uin)
	{
		QString description = (GG_EVENT_NOTIFY_DESCR == e->type)
				? QString::fromUtf8(e->event.notify_descr.descr)
				: QString();

		CurrentProtocol->socketContactStatusChanged(notify->uin, notify->status, description, 0);
		notify++;
	}
}

void GaduProtocolSocketNotifiers::handleEventNotify60(struct gg_event *e)
{
	struct gg_event_notify60 *notify = e->event.notify60;

	while (notify->uin)
	{
		CurrentProtocol->socketContactStatusChanged(notify->uin, notify->status, QString::fromUtf8(notify->descr), notify->image_size);

		notify++;
	}
}

void GaduProtocolSocketNotifiers::handleEventStatus(struct gg_event *e)
{
	if (GG_EVENT_STATUS60 == e->type)
		CurrentProtocol->socketContactStatusChanged(e->event.status60.uin, e->event.status60.status, QString::fromUtf8(e->event.status60.descr),
				e->event.status60.image_size);
	else
		CurrentProtocol->socketContactStatusChanged(e->event.status.uin, e->event.status.status, QString::fromUtf8(e->event.status.descr), 0);
}

void GaduProtocolSocketNotifiers::handleEventConnFailed(struct gg_event *e)
{
	GaduProtocol::GaduError err;

	switch (e->event.failure)
	{
		case GG_FAILURE_RESOLVING:   err = GaduProtocol::ConnectionServerNotFound; break;
		case GG_FAILURE_CONNECTING:  err = GaduProtocol::ConnectionCannotConnect; break;
		case GG_FAILURE_NEED_EMAIL:  err = GaduProtocol::ConnectionNeedEmail; break;
		case GG_FAILURE_INVALID:     err = GaduProtocol::ConnectionInvalidData; break;
		case GG_FAILURE_READING:     err = GaduProtocol::ConnectionCannotRead; break;
		case GG_FAILURE_WRITING:     err = GaduProtocol::ConnectionCannotWrite; break;
		case GG_FAILURE_PASSWORD:    err = GaduProtocol::ConnectionIncorrectPassword; break;
		case GG_FAILURE_TLS:         err = GaduProtocol::ConnectionTlsError; break;
		case GG_FAILURE_INTRUDER:    err = GaduProtocol::ConnectionIntruderError; break;
		case GG_FAILURE_UNAVAILABLE: err = GaduProtocol::ConnectionUnavailableError; break;

		default:
			kdebugm(KDEBUG_ERROR, "ERROR: unhandled/unknown connection error! %d\n", e->event.failure);
			err = GaduProtocol::ConnectionUnknow;
	}

	CurrentProtocol->socketConnFailed(err);

	// we don't have connection anymore
	watchFor(0);
}

void GaduProtocolSocketNotifiers::handleEventConnSuccess(struct gg_event *e)
{
	Q_UNUSED(e)

	CurrentProtocol->connectedToServer();
}

void GaduProtocolSocketNotifiers::handleEventDisconnect(struct gg_event *e)
{
	Q_UNUSED(e)

	// close connection
	gg_logoff(Sess);
	// we don't have connection anymore
	watchFor(0);

	CurrentProtocol->disconnectedFromServer();
}

void GaduProtocolSocketNotifiers::socketEvent()
{
	kdebugf();

	gg_event *e;
	if (!(e = gg_watch_fd(Sess)) || GG_STATE_IDLE == Sess->state)
	{
		if (e && e->type == GG_EVENT_CONN_FAILED)
			handleEventConnFailed(e);
		else
			CurrentProtocol->socketConnFailed(GaduProtocol::ConnectionUnknow);
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
	watchFor(Sess); // maybe fd has changed, we need to check always

	dumpConnectionState();
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "event: %d\n", e->type);

	switch (e->type)
	{
		case GG_EVENT_MSG:
			emit msgEventReceived(e);
			break;

		case GG_EVENT_MULTILOGON_MSG:
			emit multilogonMsgEventReceived(e);
			break;

		case GG_EVENT_MULTILOGON_INFO:
			handleEventMultilogonInfo(e);
			break;

		case GG_EVENT_TYPING_NOTIFICATION:
			emit typingNotifyEventReceived(e);
			break;

		case GG_EVENT_NOTIFY:
		case GG_EVENT_NOTIFY_DESCR:
			handleEventNotify(e);
			break;

		case GG_EVENT_NOTIFY60:
			handleEventNotify60(e);
			break;

		case GG_EVENT_STATUS:
		case GG_EVENT_STATUS60:
			handleEventStatus(e);
			break;

		case GG_EVENT_ACK:
			emit ackEventReceived(e);
			break;

		case GG_EVENT_CONN_FAILED:
			handleEventConnFailed(e);
			break;

		case GG_EVENT_CONN_SUCCESS:
			handleEventConnSuccess(e);
			break;

		case GG_EVENT_DISCONNECT:
			handleEventDisconnect(e);
			break;

		case GG_EVENT_PUBDIR50_SEARCH_REPLY:
			CurrentProtocol->CurrentSearchService->handleEventPubdir50SearchReply(e);
//			break;

		case GG_EVENT_PUBDIR50_READ:
			CurrentProtocol->CurrentPersonalInfoService->handleEventPubdir50Read(e);
			CurrentProtocol->CurrentContactPersonalInfoService->handleEventPubdir50Read(e);
//			break;

		case GG_EVENT_PUBDIR50_WRITE:
			CurrentProtocol->CurrentPersonalInfoService->handleEventPubdir50Write(e);
			break;

		case GG_EVENT_IMAGE_REQUEST:
			CurrentProtocol->CurrentChatImageService->handleEventImageRequest(e);
			break;

		case GG_EVENT_IMAGE_REPLY:
			CurrentProtocol->CurrentChatImageService->handleEventImageReply(e);
			break;

		case GG_EVENT_DCC7_NEW:
			if (!CurrentProtocol->CurrentFileTransferService)
			{
				gg_dcc7_reject(e->event.dcc7_new, GG_DCC7_REJECT_USER);
				gg_dcc7_free(e->event.dcc7_new);
				e->event.dcc7_new = NULL;
			}
			else
				CurrentProtocol->CurrentFileTransferService->handleEventDcc7New(e);
			break;

		case GG_EVENT_DCC7_ACCEPT:
			if (!CurrentProtocol->CurrentFileTransferService)
			{
				gg_dcc7_free(e->event.dcc7_accept.dcc7);
				e->event.dcc7_accept.dcc7 = NULL;
			}
			else
				CurrentProtocol->CurrentFileTransferService->handleEventDcc7Accept(e);
			break;

		case GG_EVENT_DCC7_REJECT:
			if (!CurrentProtocol->CurrentFileTransferService)
			{
				gg_dcc7_free(e->event.dcc7_reject.dcc7);
				e->event.dcc7_reject.dcc7 = NULL;
			}
			else
				CurrentProtocol->CurrentFileTransferService->handleEventDcc7Reject(e);
			break;

		case GG_EVENT_DCC7_ERROR:
			if (CurrentProtocol->CurrentFileTransferService)
				CurrentProtocol->CurrentFileTransferService->handleEventDcc7Error(e);
			break;

		case GG_EVENT_DCC7_PENDING:
			if (!CurrentProtocol->CurrentFileTransferService)
			{
				gg_dcc7_reject(e->event.dcc7_pending.dcc7, GG_DCC7_REJECT_USER);
				gg_dcc7_free(e->event.dcc7_pending.dcc7);
				e->event.dcc7_pending.dcc7 = NULL;
			}
			else
				CurrentProtocol->CurrentFileTransferService->handleEventDcc7Pending(e);
			break;

		case GG_EVENT_USERLIST100_VERSION:
			CurrentProtocol->CurrentContactListService->handleEventUserlist100Version(e);
			break;

		case GG_EVENT_USERLIST100_REPLY:
			CurrentProtocol->CurrentContactListService->handleEventUserlist100Reply(e);
			break;
	}

	gg_free_event(e);
	kdebugf2();
}

int GaduProtocolSocketNotifiers::timeout()
{
	if (!Sess)
		return -1;

	int tout = Sess->timeout;
	if (tout < 0)
		return tout;

	return qMin(tout * 1000, 15000);
}

bool GaduProtocolSocketNotifiers::handleSoftTimeout()
{
	kdebugf();

	if (!Sess || !Sess->soft_timeout)
		return false;

	Sess->timeout = 0;

	disable();
	socketEvent();
	enable();

	return true;
}

void GaduProtocolSocketNotifiers::connectionTimeout()
{
	CurrentProtocol->socketConnFailed(GaduProtocol::ConnectionTimeout);
}

#include "moc_gadu-protocol-socket-notifiers.cpp"
