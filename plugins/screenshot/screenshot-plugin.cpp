/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "debug.h"
#include "exports.h"

#include "configuration/gui/screenshot-configuration-ui-handler.h"
#include "configuration/screen-shot-configuration.h"
#include "gui/actions/screenshot-actions.h"
#include "notify/screenshot-notification.h"

#include "screenshot.h"

#include "screenshot-plugin.h"

ScreenshotPlugin::~ScreenshotPlugin()
{
}

bool ScreenshotPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	ScreenshotActions::registerActions();
	ScreenShotConfiguration::createInstance();
	ScreenShotConfigurationUiHandler::registerConfigurationUi();
	ScreenshotNotification::registerNotifications();

	return true;
}

void ScreenshotPlugin::done()
{
	kdebugf();

	ScreenshotActions::unregisterActions();
	ScreenShotConfiguration::destroyInstance();
	ScreenShotConfigurationUiHandler::unregisterConfigurationUi();
	ScreenshotNotification::unregisterNotifiactions();
}

Q_EXPORT_PLUGIN2(screenshot, ScreenshotPlugin)

#include "moc_screenshot-plugin.cpp"
