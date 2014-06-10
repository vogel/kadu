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

#include "plugin-state-manager.h"

#include "configuration/configuration-manager.h"
#include "misc/change-notifier-lock.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state-storage-09.h"
#include "plugin/state/plugin-state-storage.h"
#include "storage/storage-point-factory.h"
#include "storage/storage-point.h"

PluginStateManager::PluginStateManager(QObject *parent) :
		QObject{parent}
{
}

PluginStateManager::~PluginStateManager()
{
}

void PluginStateManager::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;
}

void PluginStateManager::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;

	if (m_pluginStateService)
		connect(&m_pluginStateService.data()->changeNotifier(), SIGNAL(changed()), this, SLOT(storePluginStatesAndFlush()));
}

void PluginStateManager::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

void PluginStateManager::loadPluginStates()
{
	if (!m_pluginDependencyHandler || !m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	bool importedFrom09 = storagePoint->loadAttribute("imported_from_09", false);
	storagePoint->storeAttribute("imported_from_09", true);

	auto pluginStates = loadPluginStates(storagePoint.get(), importedFrom09);
	auto changeNotifierLock = ChangeNotifierLock{m_pluginStateService->changeNotifier(), ChangeNotifierLock::ModeForget};
	m_pluginStateService->setPluginStates(pluginStates);
}

QMap<QString, PluginState> PluginStateManager::loadPluginStates(StoragePoint *storagePoint, bool importedFrom09) const
{
	return importedFrom09
			? PluginStateStorage{}.load(*storagePoint)
			: PluginStateStorage09{}.load(m_pluginDependencyHandler->pluginNames());
}

void PluginStateManager::storePluginStatesAndFlush()
{
	storePluginStates();
	ConfigurationManager::instance()->flush();
}

void PluginStateManager::storePluginStates()
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
