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

#include "antistring-plugin-object.h"

#include "antistring-configuration-ui-handler.h"
#include "antistring-message-filter.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message-filter-service.h"
#include "misc/paths-provider.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"

AntistringPluginObject::AntistringPluginObject(QObject *parent) :
		QObject{parent},
		m_antistringConfigurationUiHandler{}
{
}

AntistringPluginObject::~AntistringPluginObject()
{
}

void AntistringPluginObject::setAntistringConfigurationUiHandler(AntistringConfigurationUiHandler *antistringConfigurationUiHandler)
{
	m_antistringConfigurationUiHandler = antistringConfigurationUiHandler;
}

void AntistringPluginObject::setAntistringMessageFilter(AntistringMessageFilter *antistringMessageFilter)
{
	m_antistringMessageFilter = antistringMessageFilter;
}

void AntistringPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void AntistringPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void AntistringPluginObject::setMessageFilterService(MessageFilterService *messageFilterService)
{
	m_messageFilterService = messageFilterService;
}

void AntistringPluginObject::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void AntistringPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AntistringPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/antistring.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_antistringConfigurationUiHandler);
	m_messageFilterService->registerMessageFilter(m_antistringMessageFilter);
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")});
}

void AntistringPluginObject::done()
{
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")});
	m_messageFilterService->unregisterMessageFilter(m_antistringMessageFilter);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_antistringConfigurationUiHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/antistring.ui"));
}

#include "moc_antistring-plugin-object.cpp"
