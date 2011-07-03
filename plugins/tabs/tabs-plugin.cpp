/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "misc/path-conversion.h"

#include "tabs.h"

#include "tabs-plugin.h"

TabsPlugin::~TabsPlugin()
{
}

int TabsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	TabsManagerInstance = new TabsManager(this);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/tabs.ui"));
	MainConfigurationWindow::registerUiHandler(TabsManagerInstance);

	return 0;
}

void TabsPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(TabsManagerInstance);
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/tabs.ui"));
	delete TabsManagerInstance;
	TabsManagerInstance = 0;
}

Q_EXPORT_PLUGIN2(tabs, TabsPlugin)
