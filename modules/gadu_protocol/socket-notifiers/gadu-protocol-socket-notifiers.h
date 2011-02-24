/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_PROTOCOL_SOCKET_NOTIFIERS_H
#define GADU_PROTOCOL_SOCKET_NOTIFIERS_H

#include "buddies/buddy.h"

#include "gadu-protocol.h"
#include "gadu-features.h"

#include "gadu-socket-notifiers.h"

#if __GNUC__ >= 4
	#define GADU_LOCAL __attribute__ ((visibility("hidden")))
#else
	#define GADU_LOCAL
#endif /* __GNUC__ >= 4 */

#ifndef _MSC_VER
	#pragma GCC visibility push(hidden)
#endif /* _MSC_VER */

class GADU_LOCAL GaduProtocolSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	Account CurrentAccount;
	GaduProtocol *CurrentProtocol;

	gg_session *Sess;

	int Timeout;

	void dumpConnectionState();

	void handleEventMsg(struct gg_event *e);
	void handleEventNotify(struct gg_event *e);
	void handleEventNotify60(struct gg_event *e);
	void handleEventStatus(struct gg_event *e);
	void handleEventConnFailed(struct gg_event *e);
	void handleEventConnSuccess(struct gg_event *e);
	void handleEventDisconnect(struct gg_event *e);

#ifdef GADU_HAVE_MULTILOGON
	void handleEventMultilogonMsg(struct gg_event *e);
	void handleEventMultilogonInfo(struct gg_event *e);
#endif // GADU_HAVE_MULTILOGON

#ifdef GADU_HAVE_TYPING_NOTIFY
	void handleEventTypingNotify(struct gg_event *e);
#endif // GADU_HAVE_TYPING_NOTIFY

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

public:
	GaduProtocolSocketNotifiers(Account account, GaduProtocol *protocol);
	void setAccount(Account account) { CurrentAccount = account; }

	void watchFor(gg_session *sess);

};

#ifndef _MSC_VER
	#pragma GCC visibility pop
#endif /* _MSC_VER */

#endif // GADU_PROTOCOL_SOCKET_NOTIFIERS_H
