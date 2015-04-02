/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-protocol-socket-notifiers.h"

#include "services/gadu-imtoken-service.h"
#include "services/gadu-roster-service.h"
#include "services/user-data/gadu-user-data-service.h"

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include <QtCore/QSocketNotifier>
#include <libgadu.h>

#ifdef Q_OS_WIN
#	include <winsock2.h>
#else
#	include <arpa/inet.h>
#endif

GaduProtocolSocketNotifiers::GaduProtocolSocketNotifiers(Account account, GaduProtocol *protocol) :
		GaduSocketNotifiers{protocol},
		m_account{account},
		m_protocol{protocol},
		m_session{nullptr}
{
}

GaduProtocolSocketNotifiers::~GaduProtocolSocketNotifiers()
{
}

void GaduProtocolSocketNotifiers::setGaduIMTokenService(GaduIMTokenService *imTokenService)
{
	m_imTokenService = imTokenService;
}

void GaduProtocolSocketNotifiers::setGaduUserDataService(GaduUserDataService *userDataService)
{
	m_userDataService = userDataService;
}

void GaduProtocolSocketNotifiers::watchFor(gg_session *sess)
{
	m_session = sess;
	GaduSocketNotifiers::watchFor(m_session ? m_session->fd : -1);

	if (!m_session)
		m_imTokenService->setIMToken({});
}

bool GaduProtocolSocketNotifiers::checkRead()
{
	return m_session->check & GG_CHECK_READ;
}

bool GaduProtocolSocketNotifiers::checkWrite()
{
	return m_session->check & GG_CHECK_WRITE;
}

void GaduProtocolSocketNotifiers::dumpConnectionState()
{
	switch (m_session->state)
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
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "state==error! error=%d\n", m_session->error);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown state! state=%d\n", m_session->state);
			break;
	}
}

void GaduProtocolSocketNotifiers::handleEventMultilogonInfo(gg_event* e)
{
	m_protocol->CurrentMultilogonService->handleEventMultilogonInfo(e);
}

void GaduProtocolSocketNotifiers::handleEventNotify(struct gg_event *e)
{
	auto notify = (GG_EVENT_NOTIFY_DESCR == e->type)
			? e->event.notify_descr.notify
			: e->event.notify;

	while (notify->uin)
	{
		QString description = (GG_EVENT_NOTIFY_DESCR == e->type)
				? QString::fromUtf8(e->event.notify_descr.descr)
				: QString();

		m_protocol->socketContactStatusChanged(notify->uin, notify->status, description, 0);
		notify++;
	}
}

void GaduProtocolSocketNotifiers::handleEventNotify60(struct gg_event *e)
{
	auto notify = e->event.notify60;

	while (notify->uin)
	{
		m_protocol->socketContactStatusChanged(notify->uin, notify->status, QString::fromUtf8(notify->descr), notify->image_size);

		notify++;
	}
}

void GaduProtocolSocketNotifiers::handleEventStatus(struct gg_event *e)
{
	if (GG_EVENT_STATUS60 == e->type)
		m_protocol->socketContactStatusChanged(e->event.status60.uin, e->event.status60.status, QString::fromUtf8(e->event.status60.descr),
				e->event.status60.image_size);
	else
		m_protocol->socketContactStatusChanged(e->event.status.uin, e->event.status.status, QString::fromUtf8(e->event.status.descr), 0);
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
			break;
	}

	m_protocol->socketConnFailed(err);

	// we don't have connection anymore
	watchFor(nullptr);
}

void GaduProtocolSocketNotifiers::handleEventConnSuccess(struct gg_event *e)
{
	Q_UNUSED(e)

	m_protocol->connectedToServer();
}

void GaduProtocolSocketNotifiers::handleEventDisconnect(struct gg_event *e)
{
	Q_UNUSED(e)

	// close connection
	gg_logoff(m_session);
	// we don't have connection anymore
	watchFor(nullptr);

	m_protocol->disconnectedFromServer();
	m_protocol->setStatus(Status{}, SourceUser);
}

void GaduProtocolSocketNotifiers::socketEvent()
{
	kdebugf();

	auto e = gg_watch_fd(m_session);
	if (!e || GG_STATE_IDLE == m_session->state)
	{
		if (e && e->type == GG_EVENT_CONN_FAILED)
			handleEventConnFailed(e);
		else
			m_protocol->socketConnFailed(GaduProtocol::ConnectionUnknow);
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
	watchFor(m_session); // maybe fd has changed, we need to check always

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
			emit typingNotificationEventReceived(e);
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
			m_protocol->CurrentSearchService->handleEventPubdir50SearchReply(e);
//			break;

		case GG_EVENT_PUBDIR50_READ:
			m_protocol->CurrentPersonalInfoService->handleEventPubdir50Read(e);
			m_protocol->CurrentContactPersonalInfoService->handleEventPubdir50Read(e);
//			break;

		case GG_EVENT_PUBDIR50_WRITE:
			m_protocol->CurrentPersonalInfoService->handleEventPubdir50Write(e);
			break;

		case GG_EVENT_IMAGE_REQUEST:
			m_protocol->CurrentChatImageService->handleEventImageRequest(e);
			break;

		case GG_EVENT_IMAGE_REPLY:
			m_protocol->CurrentChatImageService->handleEventImageReply(e);
			break;

		case GG_EVENT_USERLIST100_VERSION:
			static_cast<GaduRosterService *>(m_protocol->rosterService())->handleEventUserlist100Version(e);
			break;

		case GG_EVENT_USERLIST100_REPLY:
			static_cast<GaduRosterService *>(m_protocol->rosterService())->handleEventUserlist100Reply(e);
			break;

		case GG_EVENT_USER_DATA:
			m_userDataService->handleUserDataEvent(e->event.user_data);
			break;

		case GG_EVENT_IMTOKEN:
			m_imTokenService->setIMToken(e->event.imtoken.imtoken);
			break;
	}

	gg_free_event(e);
	kdebugf2();
}

int GaduProtocolSocketNotifiers::timeout()
{
	if (!m_session)
		return -1;

	auto tout = m_session->timeout;
	if (tout < 0)
		return tout;

	return qMin(tout * 1000, 15000);
}

bool GaduProtocolSocketNotifiers::handleSoftTimeout()
{
	kdebugf();

	if (!m_session || !m_session->soft_timeout)
		return false;

	m_session->timeout = 0;

	disable();
	socketEvent();
	enable();

	return true;
}

void GaduProtocolSocketNotifiers::connectionTimeout()
{
	m_protocol->socketConnFailed(GaduProtocol::ConnectionTimeout);
}

#include "moc_gadu-protocol-socket-notifiers.cpp"
