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

#include "plugin-activation-service.h"

#include "misc/algorithm.h"
#include "misc/memory.h"
#include "plugin/activation/active-plugin.h"

PluginActivationService::PluginActivationService(QObject *parent) :
		QObject{parent}
{
}

PluginActivationService::~PluginActivationService()
{
}

void PluginActivationService::activatePlugin(const QString &pluginName, bool firstTime) noexcept(false)
{
	if (!contains(m_activePlugins, pluginName))
		m_activePlugins.insert(std::make_pair(pluginName, make_unique<ActivePlugin>(pluginName, firstTime)));
}

void PluginActivationService::deactivatePlugin(const QString &pluginName) noexcept
{
	m_activePlugins.erase(pluginName);
}

bool PluginActivationService::isActive(const QString &pluginName) const noexcept
{
	return contains(m_activePlugins, pluginName);
}

QSet<QString> PluginActivationService::activePlugins() const noexcept
{
	auto result = QSet<QString>{};
	for (auto const &activePlugin : m_activePlugins)
		result.insert(activePlugin.first);
	return result;
}
