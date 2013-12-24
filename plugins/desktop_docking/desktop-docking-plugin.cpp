/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "plugins/docking/docking.h"

#include "configuration/gui/desktop-dock-configuration-ui-handler.h"

#include "desktop-dock.h"

#include "desktop-docking-plugin.h"

DesktopDockingPlugin::~DesktopDockingPlugin()
{
}

bool DesktopDockingPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	DesktopDock::createInstance();
	DockingManager::instance()->setDocker(DesktopDock::instance());
	DesktopDockConfigurationUiHandler::createInstance();
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/desktop_docking.ui"));
	MainConfigurationWindow::registerUiHandler(DesktopDockConfigurationUiHandler::instance());

	return true;
}

void DesktopDockingPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(DesktopDockConfigurationUiHandler::instance());
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/desktop_docking.ui"));
	DesktopDockConfigurationUiHandler::destroyInstance();
	DockingManager::instance()->setDocker(0);
	DesktopDock::destroyInstance();
}

Q_EXPORT_PLUGIN2(desktop_docking, DesktopDockingPlugin)

#include "moc_desktop-docking-plugin.cpp"
