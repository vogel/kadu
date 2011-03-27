/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "misc/path-conversion.h"

#include "configuration/gui/freedesktop-notify-configuration-ui-handler.h"

#include "freedesktop-notify.h"

#include "freedesktop-notify-plugin.h"

FreedesktopNotifyPlugin::~FreedesktopNotifyPlugin()
{
}

int FreedesktopNotifyPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	FreedesktopNotify::createInstance();
	FreedesktopNotifyConfigurationUiHandler::registerConfigurationUi();

	return 0;
}

void FreedesktopNotifyPlugin::done()
{
	FreedesktopNotifyConfigurationUiHandler::unregisterConfigurationUi();
	FreedesktopNotify::destroyInstance();
}

Q_EXPORT_PLUGIN2(freedesktop_notify, FreedesktopNotifyPlugin)
