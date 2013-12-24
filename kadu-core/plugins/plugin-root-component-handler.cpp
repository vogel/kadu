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

#include "plugin-root-component-handler.h"

#include "plugins/plugin.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-root-component.h"

PluginRootComponentHandler::PluginRootComponentHandler(Plugin *plugin, bool firstLoad, PluginRootComponent *pluginRootComponent, QObject *parent) noexcept(false) :
		QObject{parent}, m_pluginRootComponent{pluginRootComponent}
{
	if (!m_pluginRootComponent)
		throw PluginActivationErrorException(plugin, tr("Cannot find required object in module %1.\nMaybe it's not Kadu-compatible plugin.").arg(plugin->name()));

	if (!m_pluginRootComponent->init(firstLoad))
		throw PluginActivationErrorException(plugin, tr("Module initialization routine for %1 failed.").arg(plugin->name()));
}

PluginRootComponentHandler::~PluginRootComponentHandler() noexcept
{
	m_pluginRootComponent->done();
}
