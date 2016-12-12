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

#include "sms-plugin-object.h"

#include "scripts/sms-script-manager.h"
#include "sms-actions.h"
#include "sms-configuration-ui-handler.h"
#include "sms-gateway-manager.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "misc/paths-provider.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"

SmsPluginObject::SmsPluginObject(QObject *parent) :
		QObject{parent}
{
}

SmsPluginObject::~SmsPluginObject()
{
}

void SmsPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void SmsPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void SmsPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SmsPluginObject::setSmsActions(SmsActions *smsActions)
{
	m_smsActions = smsActions;
}

void SmsPluginObject::setSmsConfigurationUiHandler(SmsConfigurationUiHandler *smsConfigurationUiHandler)
{
	m_smsConfigurationUiHandler = smsConfigurationUiHandler;
}

void SmsPluginObject::setSmsGatewayManager(SmsGatewayManager *smsGatewayManager)
{
	m_smsGatewayManager = smsGatewayManager;
}

void SmsPluginObject::setSmsScriptsManager(SmsScriptsManager *smsScriptsManager)
{
	m_smsScriptsManager = smsScriptsManager;
}

void SmsPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/sms.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_smsConfigurationUiHandler);
	m_smsGatewayManager->load();
}

void SmsPluginObject::done()
{
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_smsConfigurationUiHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/sms.ui"));
}

#include "moc_sms-plugin-object.cpp"
