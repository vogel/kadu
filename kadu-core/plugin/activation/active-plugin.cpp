/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "active-plugin.h"

#include "misc/memory.h"

ActivePlugin::ActivePlugin(const QString &pluginName, bool firstLoad) :
		m_pluginTranslationsLoader{make_unique<PluginTranslationsLoader>(pluginName)},
		m_pluginLoader{make_unique<PluginLoader>(pluginName)},
		m_pluginRootComponentHandler{make_unique<PluginRootComponentHandler>(pluginName, firstLoad, m_pluginLoader->instance())}
{
}

PluginRootComponent * ActivePlugin::pluginRootComponent() const
{
	return m_pluginRootComponentHandler->pluginRootComponent();
}
