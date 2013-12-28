/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugins/plugin-loader.h"
#include "plugins/plugin-root-component-handler.h"
#include "plugins/plugin-translations-loader.h"
#include "plugins/plugin.h"

ActivePlugin::ActivePlugin(Plugin *plugin, bool firstLoad)
{
	// Load translations before the root component of the plugin is instantiated (it is done by instance() method).
	m_pluginTranslationsLoader.reset(new PluginTranslationsLoader{plugin->name()});
	m_pluginLoader.reset(new PluginLoader{plugin->name()});
	m_pluginRootComponentHandler.reset(new PluginRootComponentHandler{plugin->name(), firstLoad, m_pluginLoader->instance()});
}
