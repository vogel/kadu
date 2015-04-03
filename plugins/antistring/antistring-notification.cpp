/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "notification/notification-manager.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"

#include "antistring-notification.h"

void AntistringNotification::registerNotifications()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")));
}

void AntistringNotification::unregisterNotifications()
{
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")));
}

void AntistringNotification::notifyStringReceived(const Chat &chat)
{
	AntistringNotification *notification = new AntistringNotification(chat);
	notification->setTitle(tr("Antistring"));
	notification->setText(tr("Your interlocutor send you love letter"));
	Core::instance()->notificationManager()->notify(notification);
}

AntistringNotification::AntistringNotification(const Chat &chat) :
		Notification(Account::null, chat, "Antistring", KaduIcon())
{
	addChatCallbacks();
}

AntistringNotification::~AntistringNotification()
{
}

#include "moc_antistring-notification.cpp"
