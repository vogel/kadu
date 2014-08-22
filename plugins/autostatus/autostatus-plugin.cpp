/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/application.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "autostatus-actions.h"
#include "autostatus.h"

#include "autostatus-plugin.h"

AutostatusPlugin::~AutostatusPlugin()
{
}

bool AutostatusPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autostatus.ui"));
	Autostatus::createInstance();
	AutostatusActions::createInstance();
	AutostatusActions::instance()->registerActions();

	return true;
}

void AutostatusPlugin::done()
{
	AutostatusActions::instance()->unregisterActions();
	AutostatusActions::destroyInstance();
	Autostatus::destroyInstance();
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autostatus.ui"));
}

Q_EXPORT_PLUGIN2(autostatus, AutostatusPlugin)

#include "moc_autostatus-plugin.cpp"
