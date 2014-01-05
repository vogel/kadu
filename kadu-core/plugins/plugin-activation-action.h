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

#include <QtCore/QString>

enum class PluginActivationType;
enum class PluginDeactivationReason;

class PluginActivationAction
{

public:
	PluginActivationAction();
	PluginActivationAction(QString pluginName, bool firstTime);
	PluginActivationAction(QString pluginName, PluginDeactivationReason deactivationReason);

	QString pluginName() const;
	PluginActivationType type() const;
	bool firstTime() const;
	PluginDeactivationReason deactivationReason() const;

private:
	QString m_pluginName;
	PluginActivationType m_type;
	union
	{
		bool m_firstTime;
		PluginDeactivationReason m_deactivationReason;
	};

};
