/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QTextDocument>

#include "accounts/account.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"
#include "notify/notify-event.h"
#include "protocols/protocol.h"

#include "status_changed_notification.h"

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

// TODO 0.6.6 what if accountData(account) == null ?
StatusChangedNotification::StatusChangedNotification(const QString &toStatus, ContactSet &contacts, Account *account) :
		AccountNotification(account, QString("StatusChanged") + toStatus,
			account->protocol()->statusPixmap(contacts.begin()->accountData(account)->status()),
			0) // TODO: 0.6.6
{
	const Contact &contact = *contacts.begin();
	Status status = contact.accountData(account)->status();
	QString syntax;

	if (!status.description().isNull())
		syntax = tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>");
	else
		syntax = tr("<b>%1</b> changed status to <i>%2</i>");

	setTitle(tr("Status changed"));
	setText(narg(syntax,
		Qt::escape(contact.display()),
		qApp->translate("@default", Status::name(status, false).toAscii().data()),
		Qt::escape(status.description())
	));
}
