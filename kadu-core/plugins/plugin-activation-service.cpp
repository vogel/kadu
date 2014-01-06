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
#include "plugins/active-plugin.h"
#include "plugins/plugins-common.h"

PluginActivationService::PluginActivationService(QObject *parent) :
		QObject{parent}
{
}

PluginActivationService::~PluginActivationService()
{
}

void PluginActivationService::activatePlugin(const QString &name, bool firstTime) noexcept(false)
{
	if (!contains(m_activePlugins, name))
		m_activePlugins.insert(std::make_pair(name, make_unique<ActivePlugin>(name, firstTime)));
}

void PluginActivationService::deactivatePlugin(const QString &name) noexcept
{
	m_activePlugins.erase(name);
}

bool PluginActivationService::isActive(const QString &name) const noexcept
{
	return contains(m_activePlugins, name);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Lists all active plugins.
 * @return list of all active plugins
 *
 * This method returns list of all active plugins. Active plugin has its shred library loaded and objects
 * created.
 */
QSet<QString> PluginActivationService::activePlugins() const noexcept
{
	auto result = QSet<QString>{};
	for (auto const &activePlugin : m_activePlugins)
		result.insert(activePlugin.first);
	return result;
}
