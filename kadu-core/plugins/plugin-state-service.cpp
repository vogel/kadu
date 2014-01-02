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

#include "plugin-state-service.h"

#include "plugins-common.h"

PluginStateService::PluginStateService(QObject *parent) :
		QObject{parent}
{
}

PluginStateService::~PluginStateService()
{
}

QMap<QString, PluginState> PluginStateService::pluginStates() const
{
	return m_pluginStates;
}

void PluginStateService::setPluginStates(const QMap<QString, PluginState> &pluginStates)
{
	m_pluginStates = pluginStates;
}

PluginState PluginStateService::pluginState(const QString &pluginName) const noexcept
{
	return m_pluginStates.contains(pluginName)
			? m_pluginStates.value(pluginName)
			: PluginState::New;
}

void PluginStateService::setPluginState(const QString &pluginName, PluginState state) noexcept
{
	if (PluginState::New == state)
		m_pluginStates.remove(pluginName);
	else
		m_pluginStates.insert(pluginName, state);
}

#include "moc_plugin-state-service.cpp"
