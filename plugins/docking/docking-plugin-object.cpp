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

#include "docking-plugin-object.h"

#include "docking.h"

#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

DockingPluginObject::DockingPluginObject(QObject *parent) :
		QObject{parent}
{
}

DockingPluginObject::~DockingPluginObject()
{
}

void DockingPluginObject::setDocking(Docking *docking)
{
	m_docking = docking;
}

void DockingPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void DockingPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/docking.ui"));
}

void DockingPluginObject::done()
{
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/docking.ui"));
}

#include "moc_docking-plugin-object.cpp"
