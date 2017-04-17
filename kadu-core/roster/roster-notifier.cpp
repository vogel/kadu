/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "roster-notifier.h"
#include "roster-notifier.moc"

#include "accounts/account.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

QString RosterNotifier::sm_rosterNotifyTopic("Roster");
QString RosterNotifier::sm_importSucceededNotifyTopic("Roster/ImportSucceeded");
QString RosterNotifier::sm_importFailedNotifyTopic("Roster/ImportFailed");
QString RosterNotifier::sm_exportSucceededNotifyTopic("Roster/ExportSucceeded");
QString RosterNotifier::sm_exportFailedNotifyTopic("Roster/ExportFailed");

RosterNotifier::RosterNotifier(QObject *parent) : QObject{parent}
{
    m_rosterNotificationEvent = NotificationEvent{sm_rosterNotifyTopic, QT_TRANSLATE_NOOP("@default", "Roster")};
    m_importSucceededNotificationEvent =
        NotificationEvent{sm_importSucceededNotifyTopic, QT_TRANSLATE_NOOP("@default", "Import from server succeeded")};
    m_importFailedNotificationEvent =
        NotificationEvent{sm_importFailedNotifyTopic, QT_TRANSLATE_NOOP("@default", "Import from server failed")};
    m_exportSucceededNotificationEvent =
        NotificationEvent{sm_exportSucceededNotifyTopic, QT_TRANSLATE_NOOP("@default", "Export to server succeeded")};
    m_exportFailedNotificationEvent =
        NotificationEvent{sm_exportFailedNotifyTopic, QT_TRANSLATE_NOOP("@default", "Export to server failed")};
}

RosterNotifier::~RosterNotifier()
{
}

void RosterNotifier::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

QList<NotificationEvent> RosterNotifier::notifyEvents()
{
    return QList<NotificationEvent>{} << m_rosterNotificationEvent << m_importSucceededNotificationEvent
                                      << m_importFailedNotificationEvent << m_exportSucceededNotificationEvent
                                      << m_exportFailedNotificationEvent;
}

void RosterNotifier::notify(const QString &topic, const Account &account, const NormalizedHtmlString &message)
{
    auto data = QVariantMap{};
    data.insert("account", qVariantFromValue(account));

    auto notification = Notification{};
    notification.type = topic;
    notification.title = tr("Roster");
    notification.text = message;
    notification.data = std::move(data);

    m_notificationService->notify(notification);
}

void RosterNotifier::notifyImportSucceeded(const Account &account)
{
    notify(
        sm_importSucceededNotifyTopic, account,
        normalizeHtml(HtmlString{tr("%1: roster import succeded")}.arg(plainToHtml(account.id()))));
}

void RosterNotifier::notifyImportFailed(const Account &account)
{
    notify(
        sm_importFailedNotifyTopic, account,
        normalizeHtml(HtmlString{tr("%1: roster import failed")}.arg(plainToHtml(account.id()))));
}

void RosterNotifier::notifyExportSucceeded(const Account &account)
{
    notify(
        sm_exportSucceededNotifyTopic, account,
        normalizeHtml(HtmlString{tr("%1: roster export succeded")}.arg(plainToHtml(account.id()))));
}

void RosterNotifier::notifyExportFailed(const Account &account)
{
    notify(
        sm_exportFailedNotifyTopic, account,
        normalizeHtml(HtmlString{tr("%1: roster export failed")}.arg(plainToHtml(account.id()))));
}
