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
#include "plugins/plugin.h"
#include "plugins/plugin-activation-action.h"
#include "plugins/plugins-common.h"

PluginActivationService::PluginActivationService(QObject *parent) :
		QObject{parent}
{
}

PluginActivationService::~PluginActivationService()
{
}

void PluginActivationService::performActivationAction(const PluginActivationAction &action) noexcept(false)
{
	switch (action.type())
	{
		case PluginActivationType::Activation:
			activatePlugin(action.plugin());
			return;

		case PluginActivationType::Deactivation:
			deactivatePlugin(action.plugin());
			return;
	}
}

bool PluginActivationService::isActive(Plugin *plugin) const noexcept
{
	return plugin ? contains(m_activePlugins, plugin->name()) : false;
}

void PluginActivationService::activatePlugin(Plugin *plugin) noexcept(false)
{
	if (!plugin)
		return;

	m_activePlugins.insert(std::make_pair(plugin->name(), make_unique<ActivePlugin>(plugin, PluginState::New == plugin->state())));
}

void PluginActivationService::deactivatePlugin(Plugin *plugin) noexcept
{
	if (!plugin)
		return;

	m_activePlugins.erase(plugin->name());
}
