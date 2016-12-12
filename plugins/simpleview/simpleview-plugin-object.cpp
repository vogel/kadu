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

#include "simpleview-plugin-object.h"

#include "simpleview.h"

#include "misc/paths-provider.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"

SimpleviewPluginObject::SimpleviewPluginObject(QObject *parent) :
		QObject{parent}
{
}

SimpleviewPluginObject::~SimpleviewPluginObject()
{
}

void SimpleviewPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void SimpleviewPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SimpleviewPluginObject::setSimpleView(SimpleView *simpleView)
{
	m_simpleView = simpleView;
}

void SimpleviewPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/simpleview.ui"));
}

void SimpleviewPluginObject::done()
{
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/simpleview.ui"));
}

#include "moc_simpleview-plugin-object.cpp"
