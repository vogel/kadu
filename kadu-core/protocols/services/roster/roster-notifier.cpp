/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify/notification/account-notification.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "roster-notifier.h"

QString RosterNotifier::RosterNotifyTopic("Roster");
QString RosterNotifier::ImportSucceededNotifyTopic("Roster/ImportSucceeded");
QString RosterNotifier::ImportFailedNotifyTopic("Roster/ImportFailed");
QString RosterNotifier::ExportSucceededNotifyTopic("Roster/ExportSucceeded");
QString RosterNotifier::ExportFailedNotifyTopic("Roster/ExportFailed");

RosterNotifier::RosterNotifier(QObject *parent) :
		QObject(parent)
{
	RosterNotifyEvent.reset(new NotifyEvent(RosterNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Roster")));
	ImportSucceededNotifyEvent.reset(new NotifyEvent(ImportSucceededNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Import from server succeeded")));
	ImportFailedNotifyEvent.reset(new NotifyEvent(ImportFailedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Import from server failed")));
	ExportSucceededNotifyEvent.reset(new NotifyEvent(ExportSucceededNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Export to server succeeded")));
	ExportFailedNotifyEvent.reset(new NotifyEvent(ExportFailedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Export to server failed")));
}

RosterNotifier::~RosterNotifier()
{
}

QList<NotifyEvent *> RosterNotifier::notifyEvents()
{
	return QList<NotifyEvent *>()
			<< RosterNotifyEvent.data()
			<< ImportSucceededNotifyEvent.data()
			<< ImportFailedNotifyEvent.data()
			<< ExportSucceededNotifyEvent.data()
			<< ExportFailedNotifyEvent.data();
}

void RosterNotifier::notify(const QString &topic, const Account &account, const QString &message)
{
	AccountNotification *notification = new AccountNotification(account, topic, KaduIcon());
	notification->setTitle(tr("Roster"));
	notification->setText(message);

	NotificationManager::instance()->notify(notification);
}

void RosterNotifier::notifyImportSucceeded(const Account &account)
{
	notify(ImportSucceededNotifyTopic, account,
		   tr("%1: roster import succeded").arg(account.id()));
}

void RosterNotifier::notifyImportFailed(const Account &account)
{
	notify(ImportFailedNotifyTopic, account,
		   tr("%1: roster import failed").arg(account.id()));
}

void RosterNotifier::notifyExportSucceeded(const Account &account)
{
	notify(ExportSucceededNotifyTopic, account,
		   tr("%1: roster export succeded").arg(account.id()));
}

void RosterNotifier::notifyExportFailed(const Account &account)
{
	notify(ExportFailedNotifyTopic, account,
		   tr("%1: roster export failed").arg(account.id()));
}

#include "moc_roster-notifier.cpp"
