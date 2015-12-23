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

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "kadu/kadu-core/misc/paths-provider.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"

AntistringPluginObject::AntistringPluginObject(QObject *parent) :
		PluginObject{parent},
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

void AntistringPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
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
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/antistring.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_antistringConfigurationUiHandler);
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")});
}

void AntistringPluginObject::done()
{
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications")});
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_antistringConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/antistring.ui"));
}

#include "moc_antistring-plugin-object.cpp"
