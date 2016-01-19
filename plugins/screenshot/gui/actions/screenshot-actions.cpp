/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "screenshot-actions.h"

#include "configuration/screen-shot-configuration.h"
#include "gui/actions/screenshot-action.h"

#include "notification/notification-manager.h"

ScreenshotActions::ScreenshotActions(QObject *parent) :
		QObject{parent}
{
}

ScreenshotActions::~ScreenshotActions()
{
}

void ScreenshotActions::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void ScreenshotActions::setScreenShotConfiguration(ScreenShotConfiguration *screenShotConfiguration)
{
	m_screenShotConfiguration = screenShotConfiguration;
}

void ScreenshotActions::init()
{
	m_screenShotActionDescription = make_owned<ScreenshotAction>(m_notificationManager, m_screenShotConfiguration, this);
}

#include "moc_screenshot-actions.cpp"
