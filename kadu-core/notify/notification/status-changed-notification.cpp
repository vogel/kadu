/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QApplication>

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "icons/kadu-icon.h"
#include "notify/notify-event.h"
#include "protocols/protocol.h"

#include "status-changed-notification.h"

NotifyEvent StatusChangedNotification::StatusChangedNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToFreeForChatNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToOnlineNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToAwayNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToNotAvailableNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToDoNotDisturbNotifyEvent;
NotifyEvent StatusChangedNotification::StatusChangedToOfflineNotifyEvent;

void StatusChangedNotification::registerEvents()
{
	StatusChangedNotifyEvent = NotifyEvent("StatusChanged", QT_TRANSLATE_NOOP("@default", "User changed status"), NotifyEvent::CallbackNotRequired);
	StatusChangedToFreeForChatNotifyEvent = NotifyEvent("StatusChanged/ToFreeForChat", QT_TRANSLATE_NOOP("@default", "to free for chat"), NotifyEvent::CallbackNotRequired);
	StatusChangedToOnlineNotifyEvent = NotifyEvent("StatusChanged/ToOnline", QT_TRANSLATE_NOOP("@default", "to online"), NotifyEvent::CallbackNotRequired);
	StatusChangedToAwayNotifyEvent = NotifyEvent("StatusChanged/ToAway", QT_TRANSLATE_NOOP("@default", "to away"), NotifyEvent::CallbackNotRequired);
	StatusChangedToNotAvailableNotifyEvent = NotifyEvent("StatusChanged/ToNotAvailable", QT_TRANSLATE_NOOP("@default", "to not available"), NotifyEvent::CallbackNotRequired);
	StatusChangedToDoNotDisturbNotifyEvent = NotifyEvent("StatusChanged/ToDoNotDisturb", QT_TRANSLATE_NOOP("@default", "to do not disturb"), NotifyEvent::CallbackNotRequired);
	StatusChangedToOfflineNotifyEvent = NotifyEvent("StatusChanged/ToOffline", QT_TRANSLATE_NOOP("@default", "to offline"), NotifyEvent::CallbackNotRequired);

	NotificationManager::instance()->registerNotifyEvent(StatusChangedNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToFreeForChatNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToOnlineNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToAwayNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToNotAvailableNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToDoNotDisturbNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToOfflineNotifyEvent);
}

void StatusChangedNotification::unregisterEvents()
{
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToFreeForChatNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOnlineNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToAwayNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToNotAvailableNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToDoNotDisturbNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOfflineNotifyEvent);
}

StatusChangedNotification::StatusChangedNotification(const QString &toStatus, const Contact &contact, const QString &statusDisplayName, const QString &description) :
		Notification(Account::null, ChatTypeContact::findChat(contact, ActionCreateAndAdd),
		QString("StatusChanged") + toStatus, contact.contactAccount().protocolHandler()->statusIcon(contact.currentStatus().type()))
{
	addChatCallbacks();

	CurrentContact = contact;
	Status status = contact.currentStatus();

	setText(tr("<b>%1</b> changed status to <i>%2</i>").arg(
			Qt::escape(contact.display(true)), Qt::escape(statusDisplayName)));

	setDetails(Qt::escape(description));

	setTitle(tr("Status changed"));
}

#include "moc_status-changed-notification.cpp"
