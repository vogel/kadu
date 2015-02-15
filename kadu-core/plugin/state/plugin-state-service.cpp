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

#include "plugin-state-service.h"

#include "misc/change-notifier.h"
#include "plugin/state/plugin-state.h"

#include <QtCore/QSet>

PluginStateService::PluginStateService(QObject *parent) noexcept :
		QObject{parent}
{
}

PluginStateService::~PluginStateService() noexcept
{
}

QMap<QString, PluginState> PluginStateService::pluginStates() const noexcept
{
	return m_pluginStates;
}

void PluginStateService::setPluginStates(QMap<QString, PluginState> pluginStates) noexcept
{
	auto setToNew = QSet<QString>{};
	for (auto &plugin : m_pluginStates.keys())
		if (!pluginStates.contains(plugin))
			setToNew.insert(plugin);

	for (auto &plugin : setToNew)
		setPluginState(plugin, PluginState::New);

	for (auto &plugin : pluginStates.keys())
		setPluginState(plugin, pluginStates.value(plugin));
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
	{
		if (m_pluginStates.contains(pluginName))
		{
			m_pluginStates.remove(pluginName);
			m_changeNotifier.notify();
			emit pluginStateChanged(pluginName, state);
		}
	}
	else
	{
		if (m_pluginStates.value(pluginName) != state)
		{
			m_pluginStates.insert(pluginName, state);
			m_changeNotifier.notify();
			emit pluginStateChanged(pluginName, state);
		}
	}
}

QList<QString> PluginStateService::enabledPlugins() noexcept
{
	return m_pluginStates.keys(PluginState::Enabled);
}

ChangeNotifier & PluginStateService::changeNotifier()
{
	return m_changeNotifier;
}

#include "moc_plugin-state-service.cpp"
