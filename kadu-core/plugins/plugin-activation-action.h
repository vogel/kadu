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

#pragma once

class Plugin;

enum class PluginActivationType;
enum class PluginActivationReason;
enum class PluginDeactivationReason;

class PluginActivationAction
{

public:
	PluginActivationAction();
	PluginActivationAction(Plugin *plugin, PluginActivationReason activationReason);
	PluginActivationAction(Plugin *plugin, PluginDeactivationReason deactivationReason);

	Plugin * plugin() const;
	PluginActivationType type() const;
	PluginActivationReason activationReason() const;
	PluginDeactivationReason deactivationReason() const;

private:
	Plugin *m_plugin;
	PluginActivationType m_type;
	union
	{
		PluginActivationReason m_activationReason;
		PluginDeactivationReason m_deactivationReason;
	};

};
