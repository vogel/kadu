/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "antistring-notification.h"

NotifyEvent * AntistringNotification::StringReceivedNotification;

void AntistringNotification::registerNotifications()
{
	StringReceivedNotification = new NotifyEvent("Antistring", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Antistring notifications"));
	NotificationManager::instance()->registerNotifyEvent(StringReceivedNotification);
}

void AntistringNotification::unregisterNotifications()
{
	NotificationManager::instance()->unregisterNotifyEvent(StringReceivedNotification);
	delete StringReceivedNotification;
	StringReceivedNotification = 0;
}

void AntistringNotification::notifyStringReceived(const Chat &chat)
{
	AntistringNotification *notification = new AntistringNotification(chat);
	notification->setTitle(tr("Antistring"));
	notification->setText(tr("Your interlocutor send you love letter"));
	NotificationManager::instance()->notify(notification);
}

AntistringNotification::AntistringNotification(const Chat &chat) :
		ChatNotification(chat, "Antistring", QString())
{
}

AntistringNotification::~AntistringNotification()
{
}
