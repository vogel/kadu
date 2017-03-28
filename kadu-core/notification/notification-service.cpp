/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification/notification.h"
#include "actions/talkable/notify-about-buddy-action.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "core/injected-factory.h"
#include "menu/menu-inventory.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-dispatcher.h"
#include "notification/notify-configuration-ui-handler.h"
#include "notification/silent-mode-service.h"
#include "parser/parser.h"
#include "widgets/chat-widget/chat-widget-manager.h"

#include "notification-service.h"

NotificationService::NotificationService(QObject *parent) : QObject{parent}
{
}

NotificationService::~NotificationService()
{
}

void NotificationService::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
    m_chatWidgetManager = chatWidgetManager;
}

void NotificationService::setConfigurationUiHandlerRepository(
    ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
    m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void NotificationService::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void NotificationService::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void NotificationService::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void NotificationService::setNotificationCallbackRepository(
    NotificationCallbackRepository *notificationCallbackRepository)
{
    m_notificationCallbackRepository = notificationCallbackRepository;
}

void NotificationService::setNotificationDispatcher(NotificationDispatcher *notificationDispatcher)
{
    m_notificationDispatcher = notificationDispatcher;
}

void NotificationService::setNotifyAboutBuddyAction(NotifyAboutBuddyAction *notifyAboutBuddyAction)
{
    m_notifyAboutBuddyAction = notifyAboutBuddyAction;
}

void NotificationService::setNotifyConfigurationUiHandler(NotifyConfigurationUiHandler *notifyConfigurationUiHandler)
{
    m_notifyConfigurationUiHandler = notifyConfigurationUiHandler;
}

void NotificationService::setParser(Parser *parser)
{
    m_parser = parser;
}

void NotificationService::setSilentModeService(SilentModeService *silentModeService)
{
    m_silentModeService = silentModeService;
}

void NotificationService::init()
{
    m_configurationUiHandlerRepository->addConfigurationUiHandler(m_notifyConfigurationUiHandler);

    auto ignoreCallback = NotificationCallback{"ignore", tr("Ignore"), [](const Notification &) {}};

    m_notificationCallbackRepository->addCallback(ignoreCallback);

    Notification::registerParserTags(m_parser);

    createActionDescriptions();
    createDefaultConfiguration();
}

void NotificationService::done()
{
    Notification::unregisterParserTags(m_parser);

    m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_notifyConfigurationUiHandler);
}

void NotificationService::createActionDescriptions()
{
    m_menuInventory->menu("buddy-list")->addAction(m_notifyAboutBuddyAction, KaduMenu::SectionActions);
}

void NotificationService::createDefaultConfiguration()
{
    m_configuration->deprecatedApi()->addVariable("Notify", "IgnoreOnlineToOnline", false);
    m_configuration->deprecatedApi()->addVariable("Notify", "NewMessageOnlyIfInactive", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void NotificationService::notify(const Notification &notification)
{
    if (m_silentModeService->isSilentOrAutoSilent() || !m_notificationDispatcher->dispatchNotification(notification))
        discardNotification(notification);
}

void NotificationService::acceptNotification(const Notification &notification)
{
    if (notification.acceptCallback.isEmpty())
    {
        auto chat = qvariant_cast<Chat>(notification.data["chat"]);
        if (chat)
            m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
    }
    else
        m_notificationCallbackRepository->callback(notification.acceptCallback).call(notification);
}

void NotificationService::discardNotification(const Notification &notification)
{
    if (!notification.discardCallback.isEmpty())
        m_notificationCallbackRepository->callback(notification.discardCallback).call(notification);
}

#undef Bool

#include "moc_notification-service.cpp"
