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

#include "actions/actions.h"
#include "configuration/screen-shot-configuration.h"
#include "gui/actions/screenshot-action.h"
#include "plugin/plugin-injected-factory.h"

ScreenshotActions::ScreenshotActions(QObject *parent) :
		QObject{parent}
{
}

ScreenshotActions::~ScreenshotActions()
{
}

void ScreenshotActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void ScreenshotActions::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void ScreenshotActions::setScreenShotConfiguration(ScreenShotConfiguration *screenShotConfiguration)
{
	m_screenShotConfiguration = screenShotConfiguration;
}

void ScreenshotActions::init()
{
	m_screenShotActionDescription = m_pluginInjectedFactory->makeOwned<ScreenshotAction>(m_screenShotConfiguration, this);
	m_actions->insert(m_screenShotActionDescription);
}

#include "moc_screenshot-actions.cpp"
