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

#include "plugins-manager.h"

#include "core/core.h"
#include "misc/kadu-paths.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-activation-error-handler.h"
#include "plugins/plugin-activation-service.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin-info-finder.h"
#include "plugins/plugin-info-repository.h"
#include "plugins/plugin-state-service.h"
#include "plugins/plugin-state-storage.h"
#include "plugins/plugin-state-storage-09.h"
#include "plugins/plugins-common.h"
#include "storage/storage-point.h"
#include "storage/storage-point-factory.h"
#include "debug.h"

PluginsManager::PluginsManager(QObject *parent) :
		QObject{parent}
{
}

PluginsManager::~PluginsManager()
{
}

void PluginsManager::setPluginInfoFinder(PluginInfoFinder *pluginInfoFinder)
{
	m_pluginInfoFinder = pluginInfoFinder;
}

void PluginsManager::setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler)
{
	m_pluginActivationErrorHandler = pluginActivationErrorHandler;
}

void PluginsManager::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

void PluginsManager::setPluginInfoRepository(PluginInfoRepository *pluginInfoRepository)
{
	m_pluginInfoRepository = pluginInfoRepository;
}

void PluginsManager::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void PluginsManager::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

void PluginsManager::initialize()
{
	loadPluginInfos();
	loadPluginStates();
	prepareDependencyGraph();
}

void PluginsManager::loadPluginInfos()
{
	if (!m_pluginInfoFinder || !m_pluginInfoRepository)
		return;

	auto pluginInfos = std::move(m_pluginInfoFinder.data()->readPluginInfos(KaduPaths::instance()->dataPath() + QLatin1String{"plugins"}));
	m_pluginInfoRepository.data()->setPluginInfos(std::move(pluginInfos));
}

void PluginsManager::loadPluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory.data()->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	bool importedFrom09 = storagePoint->loadAttribute("imported_from_09", false);
	storagePoint->storeAttribute("imported_from_09", true);

	auto pluginStates = loadPluginStates(storagePoint.get(), importedFrom09);
	m_pluginStateService.data()->setPluginStates(pluginStates);
}

QMap<QString, PluginState> PluginsManager::loadPluginStates(StoragePoint *storagePoint, bool importedFrom09) const
{
	return importedFrom09
			? PluginStateStorage{}.load(*storagePoint)
			: m_pluginInfoRepository
					? PluginStateStorage09{}.load(*m_pluginInfoRepository.data())
					: QMap<QString, PluginState>{};
}

void PluginsManager::storePluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory.data()->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	auto pluginStateStorage = PluginStateStorage{};
	auto pluginStates = m_pluginStateService.data()->pluginStates();
	pluginStateStorage.store(*storagePoint.get(), pluginStates);
}

void PluginsManager::prepareDependencyGraph()
{
	auto dependencyGraph = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*m_pluginInfoRepository.data());
	auto pluginsInDependencyCycle = dependencyGraph.get()->findPluginsInDependencyCycle();
	for (auto &pluginInDependency : pluginsInDependencyCycle)
		m_pluginInfoRepository.data()->removePluginInfo(pluginInDependency);

	m_pluginDependencyDAG = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*m_pluginInfoRepository.data());
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all protocols plugins that are enabled.
 *
 * This method activates all plugins with type "protocol" that are either enabled (PluginState::Enabled)
 * or new (PluginState::New) with attribute "load by default" set. This method is generally called before
 * any other activation to ensure that all protocols and accounts are available for other plugins.
 */
void PluginsManager::activateProtocolPlugins()
{
	for (const auto &pluginName : pluginsToActivate([](const PluginInfo &pluginInfo){ return pluginInfo.type() == "protocol"; }))
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
void PluginsManager::activatePlugins()
{
	for (const auto &pluginName : pluginsToActivate())
		activatePluginWithDependencies(pluginName);
}

QVector<QString> PluginsManager::pluginsToActivate(std::function<bool(const PluginInfo &)> filter) const
{
	auto result = QVector<QString>{};

	if (!m_pluginInfoRepository)
		return result;

	for (auto const &pluginInfo : m_pluginInfoRepository.data())
		if (filter(pluginInfo) && shouldActivate(pluginInfo))
			result.append(pluginInfo.name());

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
 *   <li>is either PluginState::Enabled or PluginState::New with PluginInfo::loadByDefault() set to true
 * </ul>
 */
bool PluginsManager::shouldActivate(const PluginInfo &pluginInfo) const noexcept
{
	if (!m_pluginStateService)
		return false;

	switch (m_pluginStateService.data()->pluginState(pluginInfo.name()))
	{
		case PluginState::Enabled:
			return true;
		case PluginState::Disabled:
			return false;
		case PluginState::New:
			return pluginInfo.loadByDefault();
	}

	return false;
}

void PluginsManager::activateReplacementPlugins()
{
	if (!m_pluginStateService)
		return;

	for (auto const &pluginToReplace : m_pluginStateService.data()->pluginsWithState(PluginState::Enabled))
	{
		if (m_pluginActivationService.data()->isActive(pluginToReplace))
			continue;

		auto replacementPlugin = findReplacementPlugin(pluginToReplace);
		if (PluginState::New == m_pluginStateService.data()->pluginState(replacementPlugin))
			activatePluginWithDependencies(replacementPlugin);
	}
}

QString PluginsManager::findReplacementPlugin(const QString &pluginToReplace) const noexcept
{
	if (!m_pluginInfoRepository)
		return {};

	for (auto const &pluginInfo : m_pluginInfoRepository.data())
		if (m_pluginInfoRepository.data()->pluginInfo(pluginInfo.name()).replaces().contains(pluginToReplace))
			return pluginInfo.name();

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
void PluginsManager::deactivatePlugins()
{
	if (!m_pluginActivationService)
		return;

	for (auto const &pluginName : m_pluginActivationService.data()->activePlugins())
		deactivatePluginWithDependents(pluginName);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns name of active plugin that provides given feature.
 * @param feature feature to search
 * @return name of active plugins that conflicts provides given feature.
 */
QString PluginsManager::findActiveProviding(const QString &feature) const
{
	if (feature.isEmpty() || !m_pluginActivationService || !m_pluginInfoRepository)
		return {};

	for (auto const &activePluginName : m_pluginActivationService.data()->activePlugins())
		if (m_pluginInfoRepository.data()->hasPluginInfo(activePluginName))
			if (m_pluginInfoRepository.data()->pluginInfo(activePluginName).provides() == feature)
				return activePluginName;

	return {};
}

QVector<QString> PluginsManager::withDependencies(const QString &pluginName) noexcept
{
	auto result = m_pluginDependencyDAG
			? m_pluginDependencyDAG.get()->findDependencies(pluginName)
			: QVector<QString>{};
	result.append(pluginName);
	return result;
}

QVector<QString> PluginsManager::withDependents(const QString &pluginName) noexcept
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
void PluginsManager::activatePluginWithDependencies(const QString &pluginName) noexcept
{
	kdebugm(KDEBUG_INFO, "activate plugin: %s\n", qPrintable(pluginName));

	if (!m_pluginActivationService || !m_pluginInfoRepository || !m_pluginStateService)
		return;

	if (m_pluginActivationService.data()->isActive(pluginName))
		return;

	try
	{
		if (m_pluginInfoRepository.data()->hasPluginInfo(pluginName))
		{
			auto conflict = findActiveProviding(m_pluginInfoRepository.data()->pluginInfo(pluginName).provides());
			if (!conflict.isEmpty())
				throw PluginActivationErrorException(pluginName, tr("Plugin %1 conflicts with: %2").arg(pluginName, conflict));
		}

		for (auto plugin : withDependencies(pluginName))
			activatePlugin(plugin);

		m_pluginStateService.data()->setPluginState(pluginName, PluginState::Enabled);
	}
	catch (PluginActivationErrorException &e)
	{
		if (m_pluginActivationErrorHandler)
			m_pluginActivationErrorHandler.data()->handleActivationError(e.pluginName(), e.errorMessage());
	}
}

void PluginsManager::activatePlugin(const QString &pluginName)
{
	if (!m_pluginStateService)
		return;

	auto state = m_pluginStateService.data()->pluginState(pluginName);
	m_pluginActivationService.data()->activatePlugin(pluginName, PluginState::New == state);
}

void PluginsManager::deactivatePluginWithDependents(const QString &pluginName) noexcept
{
	kdebugm(KDEBUG_INFO, "deactivate plugin: %s\n", qPrintable(pluginName));

	if (!m_pluginActivationService || !m_pluginActivationService.data()->isActive(pluginName))
		return;

	for (auto const &plugin : withDependents(pluginName))
		m_pluginActivationService.data()->deactivatePlugin(plugin);
}

#include "moc_plugins-manager.cpp"
