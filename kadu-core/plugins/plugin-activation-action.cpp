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

#include "plugin-activation-action.h"

#include "plugins/plugins-common.h"

PluginActivationAction::PluginActivationAction() :
		m_pluginName{}
{
}

PluginActivationAction::PluginActivationAction(QString pluginName, bool firstTime) :
		m_pluginName{std::move(pluginName)}, m_type{PluginActivationType::Activation}, m_firstTime{firstTime}
{
}

PluginActivationAction::PluginActivationAction(QString pluginName, PluginDeactivationReason deactivationReason) :
		m_pluginName{std::move(pluginName)}, m_type{PluginActivationType::Deactivation}, m_deactivationReason{deactivationReason}
{
}

QString PluginActivationAction::pluginName() const
{
	return m_pluginName;
}

PluginActivationType PluginActivationAction::type() const
{
	return m_type;
}

bool PluginActivationAction::firstTime() const
{
	return m_firstTime;
}

PluginDeactivationReason PluginActivationAction::deactivationReason() const
{
	return m_deactivationReason;
}
