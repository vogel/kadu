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

#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-loader.h"
#include "plugins/plugin-root-component.h"
#include "plugins/plugin-translations-loader.h"
#include "plugins/plugin.h"

ActivePlugin::ActivePlugin(Plugin *plugin, bool firstLoad, QObject *parent)
		: QObject{parent}, m_pluginLoader{nullptr}, m_pluginRootComponent{nullptr}
{
	m_pluginLoader.reset(new PluginLoader{plugin});
	// Load translations before the root component of the plugin is instantiated (it is done by instance() method).
	m_pluginTranslationsLoader.reset(new PluginTranslationsLoader{plugin->name()});

	m_pluginRootComponent = m_pluginLoader->instance();
	if (!m_pluginRootComponent)
		throw PluginActivationErrorException(plugin, tr("Cannot find required object in module %1.\nMaybe it's not Kadu-compatible plugin.").arg(plugin->name()));

	auto res = m_pluginRootComponent->init(firstLoad);

	if (res != 0)
	{
		m_pluginRootComponent = nullptr;
		throw PluginActivationErrorException(plugin, tr("Module initialization routine for %1 failed.").arg(plugin->name()));
	}
}

ActivePlugin::~ActivePlugin()
{
	if (m_pluginRootComponent)
		m_pluginRootComponent->done();

	m_pluginRootComponent = nullptr;
}

#include "moc_active-plugin.cpp"
