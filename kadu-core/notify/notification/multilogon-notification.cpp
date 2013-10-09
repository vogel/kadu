/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QTextDocument>

#include "multilogon/multilogon-session.h"
#include "notify/notify-event.h"
#include "protocols/protocol.h"
#include "protocols/services/multilogon-service.h"

#include "multilogon-notification.h"

NotifyEvent *MultilogonNotification::MultilogonSessionNotifyEvent = 0;
NotifyEvent *MultilogonNotification::MultilogonSessionConnectedNotifyEvent = 0;
NotifyEvent *MultilogonNotification::MultilogonSessionDisconnectedNotifyEvent = 0;

void MultilogonNotification::registerEvents()
{
	if (MultilogonSessionNotifyEvent)
		return;

	MultilogonSessionNotifyEvent = new NotifyEvent("multilogon", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Multilogon"));
	MultilogonSessionConnectedNotifyEvent = new NotifyEvent("multilogon/sessionConnected", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Multilogon session connected"));
	MultilogonSessionDisconnectedNotifyEvent = new NotifyEvent("multilogon/sessionDisconnected", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Multilogon session disconnected"));

	NotificationManager::instance()->registerNotifyEvent(MultilogonSessionNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(MultilogonSessionConnectedNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(MultilogonSessionDisconnectedNotifyEvent);
}

void MultilogonNotification::unregisterEvents()
{
	if (!MultilogonSessionNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(MultilogonSessionNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(MultilogonSessionConnectedNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(MultilogonSessionDisconnectedNotifyEvent);

	delete MultilogonSessionNotifyEvent;
	MultilogonSessionNotifyEvent = 0;

	delete MultilogonSessionConnectedNotifyEvent;
	MultilogonSessionConnectedNotifyEvent = 0;

	delete MultilogonSessionDisconnectedNotifyEvent;
	MultilogonSessionDisconnectedNotifyEvent = 0;
}

MultilogonNotification::MultilogonNotification(MultilogonSession *session, const QString &type, bool addKillCallback) :
		AccountNotification(session->account(), type, KaduIcon("kadu_icons/multilogon")), Session(session)
{
	if (addKillCallback)
	{
		addCallback(tr("Ignore"), SLOT(callbackDiscard()), "callbackDiscard()");
		addCallback(tr("Disconnect session"), SLOT(killSession()), "killSession()");

		connect(session, SIGNAL(destroyed()), this, SLOT(callbackDiscard()));
	}
}

MultilogonNotification::~MultilogonNotification()
{
}

void MultilogonNotification::killSession()
{
	if (!Session)
		return;

	Protocol *protocolHandler = Session->account().protocolHandler();
	if (!protocolHandler)
		return;

	MultilogonService *multilogonService = protocolHandler->multilogonService();
	if (!multilogonService)
		return;

	multilogonService->killSession(Session);
}


#include "moc_multilogon-notification.cpp"
