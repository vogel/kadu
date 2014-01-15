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

#include "core/core.h"
#include "misc/algorithm.h"
#include "misc/kadu-paths.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/activation/plugin-activation-error-handler.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/metadata/plugin-metadata-finder.h"
#include "plugin/state/plugin-state.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state-storage.h"
#include "plugin/state/plugin-state-storage-09.h"
#include "storage/storage-point.h"
#include "storage/storage-point-factory.h"
#include "debug.h"

PluginMetadata PluginManager::converter(PluginManager::WrappedIterator iterator)
{
	return iterator->second;
}

PluginManager::PluginManager(QObject *parent) :
		QObject{parent}
{
}

PluginManager::~PluginManager()
{
}

void PluginManager::setPluginMetadataFinder(PluginMetadataFinder *pluginMetadataFinder)
{
	m_pluginMetadataFinder = pluginMetadataFinder;
}

void PluginManager::setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler)
{
	m_pluginActivationErrorHandler = pluginActivationErrorHandler;
}

void PluginManager::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

void PluginManager::setPluginDependencyGraphBuilder(PluginDependencyGraphBuilder *pluginDependencyGraphBuilder)
{
	m_pluginDependencyGraphBuilder = pluginDependencyGraphBuilder;
}

void PluginManager::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void PluginManager::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

PluginManager::Iterator PluginManager::begin()
{
	return Iterator{m_allPluginMetadata.begin(), converter};
}

PluginManager::Iterator PluginManager::end()
{
	return Iterator{m_allPluginMetadata.end(), converter};
}

void PluginManager::initialize()
{
	loadPluginMetadata();
	prepareDependencyGraph();
	loadPluginStates();
}

void PluginManager::loadPluginMetadata()
{
	if (!m_pluginDependencyGraphBuilder || !m_pluginMetadataFinder)
		return;

	auto pluginMetatada = m_pluginMetadataFinder->readAllPluginMetadata(KaduPaths::instance()->dataPath() + QLatin1String{"plugins"});
	auto dependencyGraph = m_pluginDependencyGraphBuilder->buildGraph(pluginMetatada);
	auto pluginsInDependencyCycle = dependencyGraph.get()->findPluginsInDependencyCycle();

	std::copy_if(std::begin(pluginMetatada), std::end(pluginMetatada), std::inserter(m_allPluginMetadata, m_allPluginMetadata.begin()),
		[&pluginsInDependencyCycle](const std::map<QString, PluginMetadata>::value_type &v){ return !contains(pluginsInDependencyCycle, v.first); });
}

void PluginManager::prepareDependencyGraph()
{
	if (!m_pluginDependencyGraphBuilder)
		return;

	m_pluginDependencyDAG = m_pluginDependencyGraphBuilder->buildGraph(m_allPluginMetadata);
}

void PluginManager::loadPluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	bool importedFrom09 = storagePoint->loadAttribute("imported_from_09", false);
	storagePoint->storeAttribute("imported_from_09", true);

	auto pluginStates = loadPluginStates(storagePoint.get(), importedFrom09);
	m_pluginStateService->setPluginStates(pluginStates);
}

QMap<QString, PluginState> PluginManager::loadPluginStates(StoragePoint *storagePoint, bool importedFrom09) const
{
	return importedFrom09
			? PluginStateStorage{}.load(*storagePoint)
			: PluginStateStorage09{}.load(pluginNames());
}

void PluginManager::storePluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	auto pluginStateStorage = PluginStateStorage{};
	auto pluginStates = m_pluginStateService->pluginStates();
	pluginStateStorage.store(*storagePoint.get(), pluginStates);
}

std::set<QString> PluginManager::pluginNames() const
{
	auto result = std::set<QString>{};
	std::transform(std::begin(m_allPluginMetadata), std::end(m_allPluginMetadata), std::inserter(result, result.begin()),
		[](const std::map<QString, PluginMetadata>::value_type &v){ return v.first; });
	return result;
}

bool PluginManager::hasPluginMetadata(const QString &pluginName) const
{
	return contains(m_allPluginMetadata, pluginName);
}

PluginMetadata PluginManager::pluginMetadata(const QString &pluginName) const
{
	return m_allPluginMetadata.at(pluginName);
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

	for (auto const &plugin : m_allPluginMetadata)
		if (filter(plugin.second) && shouldActivate(plugin.second))
			result.append(plugin.first);

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
	for (auto const &possibleReplacementPlugin : m_allPluginMetadata)
		if (contains(possibleReplacementPlugin.second.replaces(), pluginToReplace))
			return possibleReplacementPlugin.first;

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

QVector<QString> PluginManager::withDependencies(const QString &pluginName) noexcept
{
	auto result = m_pluginDependencyDAG
			? m_pluginDependencyDAG.get()->findDependencies(pluginName)
			: QVector<QString>{};
	result.append(pluginName);
	return result;
}

QVector<QString> PluginManager::withDependents(const QString &pluginName) noexcept
{
	auto result = m_pluginDependencyDAG
			? m_pluginDependencyDAG.get()->findDependents(pluginName)
			: QVector<QString>{};
	result.append(pluginName);
	return result;
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

	if (!m_pluginActivationService || m_pluginActivationService->isActive(pluginName))
		return false;

	try
	{
		for (auto plugin : withDependencies(pluginName))
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
	if (!m_pluginStateService)
		return;

	if (!contains(m_allPluginMetadata, pluginName))
		throw PluginActivationErrorException(pluginName, tr("Plugin's %1 metadata not found").arg(pluginName));

	auto conflict = findActiveProviding(m_allPluginMetadata.at(pluginName).provides());
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
	if (feature.isEmpty() || !m_pluginActivationService)
		return {};

	for (auto const &activePluginName : m_pluginActivationService->activePlugins())
		if (contains(m_allPluginMetadata, activePluginName))
			if (m_allPluginMetadata.at(activePluginName).provides() == feature)
				return activePluginName;

	return {};
}

void PluginManager::deactivatePluginWithDependents(const QString &pluginName) noexcept
{
	kdebugm(KDEBUG_INFO, "deactivate plugin: %s\n", qPrintable(pluginName));

	if (!m_pluginActivationService || !m_pluginActivationService->isActive(pluginName))
		return;

	for (auto const &plugin : withDependents(pluginName))
		m_pluginActivationService->deactivatePlugin(plugin);
}

#include "moc_plugin-manager.cpp"
