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

NotifyEvent *StatusChangedNotification::StatusChangedToOnlineNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToBusyNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToInvisibleNotifyEvent = 0;
NotifyEvent *StatusChangedNotification::StatusChangedToOfflineNotifyEvent = 0;

void StatusChangedNotification::registerEvents()
{
	if (StatusChangedToOnlineNotifyEvent)
		return;

	StatusChangedToOnlineNotifyEvent = new NotifyEvent("StatusChanged/ToOnline", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "User changed status to online"));
	StatusChangedToBusyNotifyEvent = new NotifyEvent("StatusChanged/ToBusy", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "User changed status to busy"));
	StatusChangedToInvisibleNotifyEvent = new NotifyEvent("StatusChanged/ToInvisible", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "User changed status to invisible"));
	StatusChangedToOfflineNotifyEvent = new NotifyEvent("StatusChanged/ToOffline", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "User changed status to offline"));

	NotificationManager::instance()->registerNotifyEvent(StatusChangedToOnlineNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToBusyNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToInvisibleNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(StatusChangedToOfflineNotifyEvent);
}

void StatusChangedNotification::unregisterEvents()
{
	if (!StatusChangedToOnlineNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOnlineNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToBusyNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToInvisibleNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(StatusChangedToOfflineNotifyEvent);

	delete StatusChangedToOnlineNotifyEvent;
	StatusChangedToOnlineNotifyEvent = 0;

	delete StatusChangedToBusyNotifyEvent;
	StatusChangedToBusyNotifyEvent = 0;

	delete StatusChangedToInvisibleNotifyEvent;
	StatusChangedToInvisibleNotifyEvent = 0;

	delete StatusChangedToOfflineNotifyEvent;
	StatusChangedToOfflineNotifyEvent = 0;
}

// TODO 0.6.6 what if accountData(account) == null ?
StatusChangedNotification::StatusChangedNotification(const QString &toStatus, ContactSet &contacts, Account *account)
	: AccountNotification(account, QString("StatusChanged/") + toStatus, account->protocol()->statusPixmap(contacts.begin()->accountData(account)->status()), contacts)
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
