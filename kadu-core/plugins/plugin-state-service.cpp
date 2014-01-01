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

#include "plugin.h"
#include "plugin-repository.h"
#include "plugins-common.h"

PluginStateService::PluginStateService(QObject *parent) :
		QObject{parent}
{
}

PluginStateService::~PluginStateService()
{
}

void PluginStateService::setPluginRepository(PluginRepository *pluginRepository)
{
	m_pluginRepository = pluginRepository;
}

PluginState PluginStateService::pluginState(const QString &pluginName) const noexcept
{
	if (!m_pluginRepository)
		return PluginState::Disabled;

	auto pluginConfiguration = m_pluginRepository.data()->plugin(pluginName);
	return pluginConfiguration ? pluginConfiguration->state() : PluginState::Disabled;
}

void PluginStateService::setPluginState(const QString &pluginName, PluginState state) const noexcept
{
	if (!m_pluginRepository)
		return;

	auto pluginConfiguration = m_pluginRepository.data()->plugin(pluginName);
	if (pluginConfiguration)
		pluginConfiguration->setState(state);
}

#include "moc_plugin-state-service.cpp"
