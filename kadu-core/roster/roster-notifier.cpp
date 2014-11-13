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

#include "notify/notification-manager.h"
#include "notify/notification/account-notification.h"
#include "notify/notify-event.h"

#include "roster-notifier.h"

QString RosterNotifier::sm_rosterNotifyTopic("Roster");
QString RosterNotifier::sm_importSucceededNotifyTopic("Roster/ImportSucceeded");
QString RosterNotifier::sm_importFailedNotifyTopic("Roster/ImportFailed");
QString RosterNotifier::sm_exportSucceededNotifyTopic("Roster/ExportSucceeded");
QString RosterNotifier::sm_exportFailedNotifyTopic("Roster/ExportFailed");

RosterNotifier::RosterNotifier(QObject *parent) :
		QObject{parent}
{
	m_rosterNotifyEvent.reset(new NotifyEvent{sm_rosterNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Roster")});
	m_importSucceededNotifyEvent.reset(new NotifyEvent{sm_importSucceededNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Import from server succeeded")});
	m_importFailedNotifyEvent.reset(new NotifyEvent{sm_importFailedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Import from server failed")});
	m_exportSucceededNotifyEvent.reset(new NotifyEvent{sm_exportSucceededNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Export to server succeeded")});
	m_exportFailedNotifyEvent.reset(new NotifyEvent{sm_exportFailedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Export to server failed")});
}

RosterNotifier::~RosterNotifier()
{
}

QList<NotifyEvent *> RosterNotifier::notifyEvents()
{
	return QList<NotifyEvent *>{}
			<< m_rosterNotifyEvent.data()
			<< m_importSucceededNotifyEvent.data()
			<< m_importFailedNotifyEvent.data()
			<< m_exportSucceededNotifyEvent.data()
			<< m_exportFailedNotifyEvent.data();
}

void RosterNotifier::notify(const QString &topic, const Account &account, const QString &message)
{
	auto notification = new AccountNotification{account, topic, KaduIcon{}};
	notification->setTitle(tr("Roster"));
	notification->setText(message);

	NotificationManager::instance()->notify(notification);
}

void RosterNotifier::notifyImportSucceeded(const Account &account)
{
	notify(sm_importSucceededNotifyTopic, account,
		   tr("%1: roster import succeded").arg(account.id()));
}

void RosterNotifier::notifyImportFailed(const Account &account)
{
	notify(sm_importFailedNotifyTopic, account,
		   tr("%1: roster import failed").arg(account.id()));
}

void RosterNotifier::notifyExportSucceeded(const Account &account)
{
	notify(sm_exportSucceededNotifyTopic, account,
		   tr("%1: roster export succeded").arg(account.id()));
}

void RosterNotifier::notifyExportFailed(const Account &account)
{
	notify(sm_exportFailedNotifyTopic, account,
		   tr("%1: roster export failed").arg(account.id()));
}

#include "moc_roster-notifier.cpp"
