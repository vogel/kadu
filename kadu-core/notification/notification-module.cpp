/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification-module.h"

#include "notification/full-screen-service.h"
#include "notification/listener/account-event-listener.h"
#include "notification/listener/chat-event-listener.h"
#include "notification/listener/group-event-listener.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-configuration.h"
#include "notification/notification-dispatcher.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-service.h"
#include "notification/notifier-repository.h"
#include "notification/notify-configuration-importer.h"
#include "notification/notify-configuration-ui-handler.h"
#include "notification/silent-mode-service.h"

NotificationModule::NotificationModule()
{
    add_type<AccountEventListener>();
    add_type<ChatEventListener>();
    add_type<FullScreenService>();
    add_type<GroupEventListener>();
    add_type<NotificationCallbackRepository>();
    add_type<NotificationConfiguration>();
    add_type<NotificationDispatcher>();
    add_type<NotificationEventRepository>();
    add_type<NotificationService>();
    add_type<NotifierRepository>();
    add_type<NotifyConfigurationImporter>();
    add_type<NotifyConfigurationUiHandler>();
    add_type<SilentModeService>();
}

NotificationModule::~NotificationModule()
{
}
