/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"

#include "freedesktop-notify-configuration-ui-handler.h"

FreedesktopNotifyConfigurationUiHandler *FreedesktopNotifyConfigurationUiHandler::Instance = 0;

void FreedesktopNotifyConfigurationUiHandler::registerConfigurationUi()
{
	if (Instance)
		return;

	Instance = new FreedesktopNotifyConfigurationUiHandler();

	MainConfigurationWindow::registerUiFile(dataPath("plugins/configuration/freedesktop_notify.ui"));
	MainConfigurationWindow::registerUiHandler(Instance);
}

void FreedesktopNotifyConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
		MainConfigurationWindow::unregisterUiHandler(Instance);

	delete Instance;
	Instance = 0;

	MainConfigurationWindow::unregisterUiFile(dataPath("plugins/configuration/freedesktop_notify.ui"));
}

FreedesktopNotifyConfigurationUiHandler * FreedesktopNotifyConfigurationUiHandler::instance()
{
	return Instance;
}

FreedesktopNotifyConfigurationUiHandler::FreedesktopNotifyConfigurationUiHandler()
{
}

void FreedesktopNotifyConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("freedesktop-notify/customTimeout"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("freedesktop-notify/timeout"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("freedesktop-notify/showContent"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("freedesktop-notify/showContentCount"), SLOT(setEnabled(bool)));
}
