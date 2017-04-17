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

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"
#include "notification/notification.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"

#include "otr-notifier.h"
#include "otr-notifier.moc"

QString OtrNotifier::OtrNotifyTopic("OTR");
QString OtrNotifier::CreatePrivateKeyStartedNotifyTopic("OTR/CreatePrivateKeyStarted");
QString OtrNotifier::CreatePrivateKeyFinishedNotifyTopic("OTR/CreatePrivateKeyFinished");

OtrNotifier::OtrNotifier()
{
    OtrNotificationEvent = NotificationEvent{OtrNotifyTopic, QT_TRANSLATE_NOOP("@default", "OTR Encryption")};
    CreatePrivateKeyStartedNotificationEvent = NotificationEvent{
        CreatePrivateKeyStartedNotifyTopic, QT_TRANSLATE_NOOP("@default", "Create private key started")};
    CreatePrivateKeyFinishedNotificationEvent = NotificationEvent{
        CreatePrivateKeyFinishedNotifyTopic, QT_TRANSLATE_NOOP("@default", "Create private key finished")};
}

OtrNotifier::~OtrNotifier()
{
}

void OtrNotifier::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void OtrNotifier::setChatStorage(ChatStorage *chatStorage)
{
    m_chatStorage = chatStorage;
}

void OtrNotifier::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
    MyChatWidgetRepository = chatWidgetRepository;
}

void OtrNotifier::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

QList<NotificationEvent> OtrNotifier::notifyEvents()
{
    return QList<NotificationEvent>() << OtrNotificationEvent << CreatePrivateKeyStartedNotificationEvent
                                      << CreatePrivateKeyFinishedNotificationEvent;
}

void OtrNotifier::notify(const QString &topic, const Account &account, const NormalizedHtmlString &message)
{
    auto data = QVariantMap{};
    data.insert("account", qVariantFromValue(account));

    auto notification = Notification{};
    notification.type = topic;
    notification.title = (tr("OTR Encryption"));
    notification.text = message;
    notification.data = std::move(data);

    m_notificationService->notify(notification);
}

void OtrNotifier::notify(const Contact &contact, const NormalizedHtmlString &message)
{
    if (!MyChatWidgetRepository)
        return;

    auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
    auto widget = MyChatWidgetRepository->widgetForChat(chat);
    if (!widget)
        return;

    widget->appendSystemMessage(message);
}

void OtrNotifier::notifyTryingToStartSession(const Contact &contact)
{
    notify(
        contact,
        normalizeHtml(
            HtmlString{tr("%1: trying to start private conversation")}.arg(plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyTryingToRefreshSession(const Contact &contact)
{
    notify(
        contact,
        normalizeHtml(
            HtmlString{tr("%1: trying to refresh private conversation")}.arg(plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyPeerEndedSession(const Contact &contact)
{
    notify(
        contact, normalizeHtml(
                     HtmlString{tr("%1: peer ended private conversation; you should do the same")}.arg(
                         plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyGoneSecure(const Contact &contact)
{
    notify(
        contact,
        normalizeHtml(HtmlString{tr("%1: private conversation started")}.arg(plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyGoneInsecure(const Contact &contact)
{
    notify(
        contact,
        normalizeHtml(HtmlString{tr("%1: private conversation stopped")}.arg(plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyStillSecure(const Contact &contact)
{
    notify(
        contact,
        normalizeHtml(HtmlString{tr("%1: conversation is still private")}.arg(plainToHtml(contact.display(true)))));
}

void OtrNotifier::notifyCreatePrivateKeyStarted(const Account &account)
{
    notify(
        CreatePrivateKeyStartedNotifyTopic, account,
        normalizeHtml(
            HtmlString{tr("%1: creating private key, it can took a few minutes")}.arg(plainToHtml(account.id()))));
}

void OtrNotifier::notifyCreatePrivateKeyFinished(const Account &account, bool ok)
{
    notify(
        CreatePrivateKeyFinishedNotifyTopic, account,
        ok ? normalizeHtml(
                 HtmlString{tr("%1: private key created, you can start a private conversation now")}.arg(
                     plainToHtml(account.id())))
           : normalizeHtml(HtmlString{tr("%1: private key creation failed")}.arg(plainToHtml(account.id()))));
}
