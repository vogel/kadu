/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QTextDocument>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "icons/icons-manager.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-manager.h"
#include "notification/notification-event.h"

#include "firewall-notification.h"

void FirewallNotification::registerNotifications()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("firewallNotification", QT_TRANSLATE_NOOP("@default", "Message was firewalled")));
}

void FirewallNotification::unregisterNotifications()
{
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("firewallNotification", QT_TRANSLATE_NOOP("@default", "Message was firewalled")));
}

void FirewallNotification::notify(const Chat &chat, const Contact &sender, const QString &message)
{
	FirewallNotification *notification = new FirewallNotification(chat);
	notification->setTitle(tr("Message was blocked"));
	notification->setText(Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "notification_syntax",
		tr("%u writes")).replace("%u", Qt::escape(sender.display(true))).remove("%m"));
	notification->setDetails(Qt::escape(message));
	Core::instance()->notificationManager()->notify(notification);
}

FirewallNotification::FirewallNotification(const Chat &chat) :
		Notification(Account::null, chat, "firewallNotification", KaduIcon("ManageKeysWindowIcon"))
{
	addChatCallbacks();
}

FirewallNotification::~FirewallNotification()
{

}

#include "moc_firewall-notification.cpp"
