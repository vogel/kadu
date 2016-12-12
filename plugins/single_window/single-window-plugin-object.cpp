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

#include "single-window-plugin-object.h"

#include "single-window.h"

#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"
#include "misc/paths-provider.h"

SingleWindowPluginObject::SingleWindowPluginObject(QObject *parent) :
		QObject{parent}
{
}

SingleWindowPluginObject::~SingleWindowPluginObject()
{
}

void SingleWindowPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void SingleWindowPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SingleWindowPluginObject::setSingleWindowManager(SingleWindowManager *singleWindowManager)
{
	m_singleWindowManager = singleWindowManager;
}

void SingleWindowPluginObject::setSingleWindow(SingleWindow *singleWindow)
{
	m_singleWindow = singleWindow;
}

void SingleWindowPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/single_window.ui"));
}

void SingleWindowPluginObject::done()
{
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/single_window.ui"));
}

#include "moc_single-window-plugin-object.cpp"
