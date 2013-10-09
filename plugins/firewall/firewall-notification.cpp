/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "configuration/configuration-file.h"
#include "icons/icons-manager.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "firewall-notification.h"

NotifyEvent * FirewallNotification::FirewalledNotification = 0;

void FirewallNotification::registerNotifications()
{
	if (FirewalledNotification)
		return;

	FirewalledNotification = new NotifyEvent("firewallNotification", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Message was firewalled"));
	NotificationManager::instance()->registerNotifyEvent(FirewalledNotification);
}

void FirewallNotification::unregisterNotifications()
{
	if (!FirewalledNotification)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(FirewalledNotification);
	delete FirewalledNotification;
	FirewalledNotification = 0;
}

void FirewallNotification::notify(const Chat &chat, const Contact &sender, const QString &message)
{
	FirewallNotification *notification = new FirewallNotification(chat);
	notification->setTitle(tr("Message was blocked"));
	notification->setText(config_file.readEntry("Firewall", "notification_syntax",
		tr("%u writes")).replace("%u", Qt::escape(sender.display(true))).remove("%m"));
	notification->setDetails(Qt::escape(message));
	NotificationManager::instance()->notify(notification);
}

FirewallNotification::FirewallNotification(const Chat &chat) :
		ChatNotification(chat, "firewallNotification", KaduIcon("ManageKeysWindowIcon"))
{
}

FirewallNotification::~FirewallNotification()
{

}

#include "moc_firewall-notification.cpp"
