/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-state-storage-09.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "misc/algorithm.h"
#include "plugin/state/plugin-state.h"

#include <QtCore/QSet>

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Import 0.9.x configuration.
 *
 * This method loads old configuration from depreceated configuration entries: General/EverLaoded,
 * General/LoadedModules and General/UnloadedModules.
 */
QMap<QString, PluginState> PluginStateStorage09::load(const ::std::set<QString> &installedPluginNames) const
{
	auto result = QMap<QString, PluginState>{};

	auto everLoaded = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts).toSet();
	auto loaded = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "LoadedModules");

	auto loadedPlugins = loaded.split(',', QString::SkipEmptyParts).toSet();
	everLoaded += loadedPlugins;
	auto unloaded_str = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "UnloadedModules");
	auto unloadedPlugins = unloaded_str.split(',', QString::SkipEmptyParts).toSet();

	auto allPlugins = everLoaded + unloadedPlugins; // just in case...
	QSet<QString> oldPlugins;
	for (auto pluginName : allPlugins)
		if (!contains(installedPluginNames, pluginName) && !oldPlugins.contains(pluginName))
			oldPlugins.insert(pluginName);

	if (loadedPlugins.contains("encryption"))
	{
		loadedPlugins.remove("encryption");
		loadedPlugins.insert("encryption_ng");
		loadedPlugins.insert("encryption_ng_simlite");
	}
	if (loadedPlugins.contains("osd_hints"))
	{
		loadedPlugins.remove("osd_hints");
		loadedPlugins.insert("hints");
	}

	for (auto const &pluginName : installedPluginNames)
		if (allPlugins.contains(pluginName))
		{
			if (loadedPlugins.contains(pluginName))
				result.insert(pluginName, PluginState::Enabled);
			else if (everLoaded.contains(pluginName))
				result.insert(pluginName, PluginState::Disabled);
		}

	for (auto pluginName : oldPlugins)
		if (allPlugins.contains(pluginName))
		{
			if (loadedPlugins.contains(pluginName))
				result.insert(pluginName, PluginState::Enabled);
			else if (everLoaded.contains(pluginName))
				result.insert(pluginName, PluginState::Disabled);
		}

	return result;
}
