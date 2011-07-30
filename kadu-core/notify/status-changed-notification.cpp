/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QApplication>
#include <QtGui/QTextDocument>

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "icons/kadu-icon.h"
#include "notify/notify-event.h"
#include "protocols/protocol.h"

#include "status-changed-notification.h"

NotifyEvent *StatusChangedNotification::StatusChangedNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToFreeForChatNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToOnlineNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToAwayNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToNotAvailableNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToDoNotDisturbNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToOfflineNotifyEvent = 0;

void StatusChangedNotification::registerEvents()
{
	if (StatusChangedToOnlineNotifyEvent)
		return;

	StatusChangedNotifyEvent = new NotifyEvent("StatusChanged", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "User changed status"));
	StatusChangedToFreeForChatNotifyEvent = new NotifyEvent("StatusChanged/ToFreeForChat", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to free for chat"));
	StatusChangedToOnlineNotifyEvent = new NotifyEvent("StatusChanged/ToOnline", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to online"));
	StatusChangedToAwayNotifyEvent = new NotifyEvent("StatusChanged/ToAway", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to away"));
	StatusChangedToNotAvailableNotifyEvent = new NotifyEvent("StatusChanged/ToNotAvailable", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to not available"));
	StatusChangedToDoNotDisturbNotifyEvent = new NotifyEvent("StatusChanged/ToDoNotDisturb", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to do not disturb"));
	StatusChangedToOfflineNotifyEvent = new NotifyEvent("StatusChanged/ToOffline", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "to offline"));

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
	if (!StatusChangedToOnlineNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToFreeForChatNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOnlineNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToAwayNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToNotAvailableNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToDoNotDisturbNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOfflineNotifyEvent);

	delete StatusChangedNotifyEvent;
	StatusChangedNotifyEvent = 0;

	delete StatusChangedToFreeForChatNotifyEvent;
	StatusChangedToFreeForChatNotifyEvent = 0;

	delete StatusChangedToOnlineNotifyEvent;
	StatusChangedToOnlineNotifyEvent = 0;

	delete StatusChangedToAwayNotifyEvent;
	StatusChangedToAwayNotifyEvent = 0;

	delete StatusChangedToNotAvailableNotifyEvent;
	StatusChangedToNotAvailableNotifyEvent = 0;

	delete StatusChangedToDoNotDisturbNotifyEvent;
	StatusChangedToDoNotDisturbNotifyEvent = 0;

	delete StatusChangedToOfflineNotifyEvent;
	StatusChangedToOfflineNotifyEvent = 0;
}

StatusChangedNotification::StatusChangedNotification(const QString &toStatus, const Contact &contact) :
		ChatNotification(ChatManager::instance()->findChat(ContactSet(contact)), QString("StatusChanged") + toStatus,
			contact.contactAccount().protocolHandler()->statusIcon(contact.currentStatus().type()))
{
	Status status = contact.currentStatus();

	setText(tr("<b>%1</b> changed status to <i>%2</i>").arg(
			Qt::escape(contact.ownerBuddy().display()), status.displayName()));

	setDetails(Qt::escape(status.description()));

	setTitle(tr("Status changed"));
}
