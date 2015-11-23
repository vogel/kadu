/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "multilogon/multilogon-session.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"
#include "protocols/protocol.h"
#include "protocols/services/multilogon-service.h"

#include "multilogon-notification.h"

void MultilogonNotification::registerEvents()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("multilogon", QT_TRANSLATE_NOOP("@default", "Multilogon")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("multilogon/sessionConnected", QT_TRANSLATE_NOOP("@default", "Multilogon session connected")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("multilogon/sessionDisconnected", QT_TRANSLATE_NOOP("@default", "Multilogon session disconnected")));

	auto multilogonDisconnect = NotificationCallback{
		"multilogon-disconnect",
		tr("Disconnect session"),
		[](Notification *notification){
			auto multilogonNotification = qobject_cast<MultilogonNotification *>(notification);
			if (multilogonNotification)
				multilogonNotification->killSession();
		}
	};
	Core::instance()->notificationCallbackRepository()->addCallback(multilogonDisconnect);
}

void MultilogonNotification::unregisterEvents()
{
	if (Core::instance()) // TODO: hack
	{
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("multilogon", QT_TRANSLATE_NOOP("@default", "Multilogon")));
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("multilogon/sessionConnected", QT_TRANSLATE_NOOP("@default", "Multilogon session connected")));
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("multilogon/sessionDisconnected", QT_TRANSLATE_NOOP("@default", "Multilogon session disconnected")));
	}
}

MultilogonNotification::MultilogonNotification(MultilogonSession *session, const QString &type, bool addKillCallback) :
		Notification(session->account(), Chat::null, type, KaduIcon("kadu_icons/multilogon")), Session(session)
{
	if (addKillCallback)
	{
		addCallback("ignore");
		addCallback("multilogon-disconnect");

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
