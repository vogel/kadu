/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin-manager.h"

#include "misc/algorithm.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state-storage-09.h"
#include "plugin/state/plugin-state-storage.h"
#include "plugin/state/plugin-state.h"
#include "debug.h"

PluginManager::PluginManager(QObject *parent) :
		QObject{parent}
{
	m_runningUnity = QString::compare(qgetenv("XDG_CURRENT_DESKTOP"), "unity", Qt::CaseInsensitive) == 0;
}

PluginManager::~PluginManager()
{
}

void PluginManager::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

void PluginManager::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;
}

void PluginManager::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void PluginManager::activateProtocolPlugins()
{
	if (m_pluginActivationService)
		for (const auto &pluginName : pluginsToActivate([](const PluginMetadata &pluginMetadata){ return pluginMetadata.type() == "protocol"; }))
			m_pluginActivationService->activatePluginWithDependencies(pluginName);
}

void PluginManager::activatePlugins()
{
	if (m_pluginActivationService)
		for (const auto &pluginName : pluginsToActivate())
		{
			m_pluginActivationService->activatePluginWithDependencies(pluginName);
			m_pluginStateService->setPluginState(pluginName, PluginState::Enabled); // make plugin enabled if it was new
		}
}

QVector<QString> PluginManager::pluginsToActivate(std::function<bool(const PluginMetadata &)> filter) const
{
	auto result = QVector<QString>{};
	if (!m_pluginDependencyHandler)
		return result;

	for (auto const &plugin : m_pluginDependencyHandler)
		if (filter(plugin) && shouldActivate(plugin))
			result.append(plugin.name());

	return result;
}

bool PluginManager::shouldActivate(const PluginMetadata &pluginMetadata) const noexcept
{
	if (!m_pluginStateService)
		return false;

	if (m_runningUnity)
	{
		if (pluginMetadata.name().contains("indicator_docking"))
			return true;
		if (pluginMetadata.name().contains("docking"))
			return false;
	}

	switch (m_pluginStateService->pluginState(pluginMetadata.name()))
	{
		case PluginState::Enabled:
			return true;
		case PluginState::Disabled:
			return false;
		case PluginState::New:
			return pluginMetadata.loadByDefault();
	}

	return false;
}

void PluginManager::activateReplacementPlugins()
{
	if (!m_pluginActivationService || !m_pluginStateService)
		return;

	for (auto const &pluginToReplace : m_pluginStateService->enabledPlugins())
	{
		if (m_pluginActivationService->isActive(pluginToReplace))
			continue;

		auto replacementPlugin = findReplacementPlugin(pluginToReplace);
		if (replacementPlugin.isEmpty())
			continue;

		if (PluginState::New == m_pluginStateService->pluginState(replacementPlugin))
			if (m_pluginActivationService->activatePluginWithDependencies(replacementPlugin).contains(replacementPlugin))
			{
				m_pluginStateService->setPluginState(pluginToReplace, PluginState::Disabled);
				m_pluginStateService->setPluginState(replacementPlugin, PluginState::Enabled);
			}
	}
}

QString PluginManager::findReplacementPlugin(const QString &pluginToReplace) const noexcept
{
	if (!m_pluginDependencyHandler)
		return {};

	for (auto const &possibleReplacementPlugin : m_pluginDependencyHandler)
		if (contains(possibleReplacementPlugin.replaces(), pluginToReplace))
			return possibleReplacementPlugin.name();

	return {};
}

void PluginManager::deactivatePlugins()
{
	if (!m_pluginActivationService)
		return;

	for (auto const &pluginName : m_pluginActivationService->activePlugins())
		m_pluginActivationService->deactivatePluginWithDependents(pluginName);
}

#include "moc_plugin-manager.cpp"
