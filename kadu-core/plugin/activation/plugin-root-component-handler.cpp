/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/plugin-root-component.h"

PluginRootComponentHandler::PluginRootComponentHandler(const QString &pluginName, bool firstLoad, PluginRootComponent *pluginRootComponent, QObject *parent) noexcept(false) :
		QObject{parent}, m_pluginRootComponent{pluginRootComponent}
{
	if (!m_pluginRootComponent)
		throw PluginActivationErrorException{pluginName, tr("Cannot find required object in plugin %1.\nMaybe it's not Kadu-compatible plugin.").arg(pluginName)};

	if (!m_pluginRootComponent->init(firstLoad))
		throw PluginActivationErrorException{pluginName, tr("Plugin initialization routine for %1 failed.").arg(pluginName)};
}

PluginRootComponentHandler::~PluginRootComponentHandler() noexcept
{
	m_pluginRootComponent->done();
}

PluginRootComponent * PluginRootComponentHandler::pluginRootComponent() const
{
	return m_pluginRootComponent;
}
