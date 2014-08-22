/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/gui/freedesktop-notify-configuration-ui-handler.h"

#include "freedesktop-notify.h"

#include "freedesktop-notify-plugin.h"

FreedesktopNotifyPlugin::~FreedesktopNotifyPlugin()
{
}

bool FreedesktopNotifyPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	FreedesktopNotify::createInstance();
	FreedesktopNotifyConfigurationUiHandler::registerConfigurationUi();

	return true;
}

void FreedesktopNotifyPlugin::done()
{
	FreedesktopNotifyConfigurationUiHandler::unregisterConfigurationUi();
	FreedesktopNotify::destroyInstance();
}

Q_EXPORT_PLUGIN2(freedesktop_notify, FreedesktopNotifyPlugin)

#include "moc_freedesktop-notify-plugin.cpp"
