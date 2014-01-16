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

#pragma once

#include "exports.h"

#include <QtCore/QMap>

class StoragePoint;

enum class PluginState;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginStateStorage
 * @short Loads and stores states of plugins.
 * @todo Split into two classes which can be unit-tested.
 */
class KADUAPI PluginStateStorage
{

public:
	/**
	 * @return Configuration of plugin states loaded from storage.
	 * @param storagePoint storage to load configuration from
	 */
	QMap<QString, PluginState> load(StoragePoint &storagePoint) const;

	/**
	 * @short Stores new configuration of plugin states in storage.
	 * @param storagePoint storage to store configuration to
	 * @param pluginStates new configuration of plugin states
	 *
	 * All previously data stored in provided \p storagePoint is removed.
	 */
	void store(StoragePoint &storagePoint, const QMap<QString, PluginState> &pluginStates) const;

};

/**
 * @}
 */
