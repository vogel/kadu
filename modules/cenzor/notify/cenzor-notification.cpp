/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "icons-manager.h"

#include "cenzor-notification.h"

NotifyEvent * CenzorNotification::CenzoredNotification = 0;

void CenzorNotification::registerNotifications()
{
	if (CenzoredNotification)
		return;

	CenzoredNotification = new NotifyEvent("cenzorNotification", NotifyEvent::CallbackNotRequired, "Message was cenzored");
	NotificationManager::instance()->registerNotifyEvent(CenzoredNotification);
}

void CenzorNotification::unregisterNotifiactions()
{
	if (!CenzoredNotification)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(CenzoredNotification);
	delete CenzoredNotification;
	CenzoredNotification = 0;
}

void CenzorNotification::notifyCenzored(const Chat &chat)
{
	CenzorNotification *notification = new CenzorNotification(chat);
	notification->setTitle(tr("Message was cenzored"));
	notification->setText("Cenzor");
	notification->setDetails(tr("Your interlocutor used obscene word and became admonished"));
	NotificationManager::instance()->notify(notification);
}

CenzorNotification::CenzorNotification(const Chat &chat) :
		ChatNotification(chat, "cenzorNotification", QString())
{
}

CenzorNotification::~CenzorNotification()
{

}
