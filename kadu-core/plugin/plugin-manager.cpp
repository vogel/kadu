/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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
#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/activation/plugin-activation-error-handler.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/state/plugin-state.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state-storage.h"
#include "plugin/state/plugin-state-storage-09.h"
#include "debug.h"

PluginManager::PluginManager(QObject *parent) :
		QObject{parent}
{
}

PluginManager::~PluginManager()
{
}

void PluginManager::setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler)
{
	m_pluginActivationErrorHandler = pluginActivationErrorHandler;
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

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all protocols plugins that are enabled.
 *
 * This method activates all plugins with type "protocol" that are either enabled (PluginState::Enabled)
 * or new (PluginState::New) with attribute "load by default" set. This method is generally called before
 * any other activation to ensure that all protocols and accounts are available for other plugins.
 */
void PluginManager::activateProtocolPlugins()
{
	for (const auto &pluginName : pluginsToActivate([](const PluginMetadata &pluginMetadata){ return pluginMetadata.type() == "protocol"; }))
		activatePluginWithDependencies(pluginName);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method activates all plugins that are either enabled (PluginState::Enabled) or new (PluginState::New)
 * with attribute "load by default" set. If given enabled plugin is no longer available replacement plugin is searched
 * (by checking Plugin::replaces()). Any found replacement plugin is activated.
 */
void PluginManager::activatePlugins()
{
	for (const auto &pluginName : pluginsToActivate())
		activatePluginWithDependencies(pluginName);
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

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true if this plugin should be activated.
 * @return true if this plugin should be activated
 *
 * Module should be activated only if:
 * <ul>
 *   <li>it is valid (has .desc file associated with it)
 *   <li>is either PluginState::Enabled or PluginState::New with PluginMetadata::loadByDefault() set to true
 * </ul>
 */
bool PluginManager::shouldActivate(const PluginMetadata &pluginMetadata) const noexcept
{
	if (!m_pluginStateService)
		return false;

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
	if (!m_pluginStateService)
		return;

	for (auto const &pluginToReplace : m_pluginStateService->enabledPlugins())
	{
		if (m_pluginActivationService->isActive(pluginToReplace))
			continue;

		auto replacementPlugin = findReplacementPlugin(pluginToReplace);
		if (PluginState::New == m_pluginStateService->pluginState(replacementPlugin))
			if (activatePluginWithDependencies(replacementPlugin))
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

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method deactivated all active plugins. First iteration of deactivation check Plugin::usageCounter() value
 * to check if given plugin can be safely removed (no other active plugins depends on it). This procedure is
 * performed for all active plugins until no more plugins can be deactivated. Then second iteration is performed.
 * This time no checks are performed.
 */
void PluginManager::deactivatePlugins()
{
	if (!m_pluginActivationService)
		return;

	for (auto const &pluginName : m_pluginActivationService->activePlugins())
		deactivatePluginWithDependents(pluginName);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activates given plugin and all its dependencies.
 * @param plugin plugin to activate
 * @return true, if plugin was successfully activated
 *
 * This method activates given plugin and all its dependencies. Plugin can be activated only when no conflict
 * is found and all dependencies can be activated. In other case false is returned and plugin will not be activated.
 * Please note that no dependency plugin activated in this method will be automatically deactivated if
 * this method fails, so list of active plugins can be changed even if plugin could not be activated.
 *
 * After successfull activation all dependencies are locked using incDependenciesUsageCount() and cannot be
 * deactivated without deactivating plugin. Plugin::usageCounter() of dependencies is increased.
 */
bool PluginManager::activatePluginWithDependencies(const QString &pluginName) noexcept
{
	kdebugm(KDEBUG_INFO, "activate plugin: %s\n", qPrintable(pluginName));

	if (!m_pluginActivationService || m_pluginActivationService->isActive(pluginName) || !m_pluginDependencyHandler)
		return false;

	try
	{
		for (auto plugin : m_pluginDependencyHandler->withDependencies(pluginName))
			activatePlugin(plugin);
	}
	catch (PluginActivationErrorException &e)
	{
		if (m_pluginActivationErrorHandler)
			m_pluginActivationErrorHandler->handleActivationError(e.pluginName(), e.errorMessage());
		return false;
	}

	return true;
}

void PluginManager::activatePlugin(const QString &pluginName) noexcept(false)
{
	if (!m_pluginStateService || !m_pluginDependencyHandler)
		return;

	if (!m_pluginDependencyHandler->hasPluginMetadata(pluginName))
		throw PluginActivationErrorException(pluginName, tr("Plugin's %1 metadata not found").arg(pluginName));

	auto conflict = findActiveProviding(m_pluginDependencyHandler->pluginMetadata(pluginName).provides());
	if (!conflict.isEmpty())
		throw PluginActivationErrorException(pluginName, tr("Plugin %1 conflicts with: %2").arg(pluginName, conflict));

	auto state = m_pluginStateService->pluginState(pluginName);
	m_pluginActivationService->activatePlugin(pluginName, PluginState::New == state);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns name of active plugin that provides given feature.
 * @param feature feature to search
 * @return name of active plugins that conflicts provides given feature.
 */
QString PluginManager::findActiveProviding(const QString &feature) const
{
	if (feature.isEmpty() || !m_pluginActivationService || !m_pluginDependencyHandler)
		return {};

	for (auto const &activePluginName : m_pluginActivationService->activePlugins())
		if (m_pluginDependencyHandler->hasPluginMetadata(activePluginName))
			if (m_pluginDependencyHandler->pluginMetadata(activePluginName).provides() == feature)
				return activePluginName;

	return {};
}

void PluginManager::deactivatePluginWithDependents(const QString &pluginName) noexcept
{
	kdebugm(KDEBUG_INFO, "deactivate plugin: %s\n", qPrintable(pluginName));

	if (!m_pluginActivationService || !m_pluginActivationService->isActive(pluginName) || !m_pluginDependencyHandler)
		return;

	for (auto const &plugin : m_pluginDependencyHandler->withDependents(pluginName))
		m_pluginActivationService->deactivatePlugin(plugin);
}

#include "moc_plugin-manager.cpp"
