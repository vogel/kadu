/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "main-configuration-window-service.h"

#include "configuration/config-file-data-manager.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "configuration/gui/configuration-ui-handler.h"
#include "core/injected-factory.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"

MainConfigurationWindowService::MainConfigurationWindowService(QObject *parent) :
		QObject{parent}
{
}

MainConfigurationWindowService::~MainConfigurationWindowService()
{
}


void MainConfigurationWindowService::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void MainConfigurationWindowService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void MainConfigurationWindowService::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void MainConfigurationWindowService::registerUiFile(const QString &uiFile)
{
	m_uiFiles.append(uiFile);
	if (m_mainConfigurationWindow)
		m_mainConfigurationWindow->widget()->appendUiFile(uiFile);
}

void MainConfigurationWindowService::unregisterUiFile(const QString &uiFile)
{
	m_uiFiles.removeAll(uiFile);
	if (m_mainConfigurationWindow)
		m_mainConfigurationWindow->widget()->removeUiFile(uiFile);
}

void MainConfigurationWindowService::show()
{
	if (!m_mainConfigurationWindow)
	{
		auto dataManager = m_injectedFactory->makeInjected<ConfigFileDataManager>();
		m_mainConfigurationWindow = m_injectedFactory->makeInjected<MainConfigurationWindow>(dataManager, m_kaduWindowService->kaduWindow());
		for (auto const &uiFile : m_uiFiles)
			m_mainConfigurationWindow->widget()->appendUiFile(uiFile);
		for (auto configurationUiHandler : m_configurationUiHandlerRepository)
			configurationUiHandler->mainConfigurationWindowCreated(m_mainConfigurationWindow);
	}

	m_mainConfigurationWindow->show();
}
