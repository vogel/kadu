/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-repository.h"

#include "plugin/activation/active-plugin.h"
#include "plugin/activation/plugin-activation-service.h"

PluginRepository::PluginRepository(QObject *parent) :
		QObject{parent}
{
}

PluginRepository::~PluginRepository()
{
}

void PluginRepository::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

PluginRootComponent * PluginRepository::plugin(const QString& pluginName) const
{
	if (auto a = m_pluginActivationService->activePlugin(pluginName))
		return a->pluginRootComponent();
	else
		return nullptr;
}
