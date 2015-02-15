/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FREEDESKTOP_NOTIFY_CONFIGURATION_UI_HANDLER_H
#define FREEDESKTOP_NOTIFY_CONFIGURATION_UI_HANDLER_H

#include "gui/windows/main-configuration-window.h"

class FreedesktopNotifyConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(FreedesktopNotifyConfigurationUiHandler)

	static FreedesktopNotifyConfigurationUiHandler *Instance;

	FreedesktopNotifyConfigurationUiHandler();

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

	static FreedesktopNotifyConfigurationUiHandler * instance();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // FREEDESKTOP_NOTIFY_CONFIGURATION_UI_HANDLER_H
