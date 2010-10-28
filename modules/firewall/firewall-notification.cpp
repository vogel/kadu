/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "configuration/configuration-file.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "icons-manager.h"

#include "firewall-notification.h"

NotifyEvent * FirewallNotification::FirewalledNotification = 0;

void FirewallNotification::registerNotifications()
{
	if (FirewalledNotification)
		return;

	FirewalledNotification = new NotifyEvent("firewallNotification", NotifyEvent::CallbackNotRequired, "Message was firewalled");
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

void FirewallNotification::notify(Chat chat, Contact sender, const QString &message)
{
	FirewallNotification *notification = new FirewallNotification(chat);
	notification->setTitle(tr("Message was blocked"));
	notification->setText(config_file.readEntry("Firewall", "notification_syntax",
		tr("%u writes")).replace("%u", sender.ownerBuddy().display()).remove("%m"));
	notification->setDetails(message);
	NotificationManager::instance()->notify(notification);
}

FirewallNotification::FirewallNotification(Chat chat) :
		ChatNotification(chat, "firewallNotification", "ManageKeysWindowIcon")
{
}

FirewallNotification::~FirewallNotification()
{

}
