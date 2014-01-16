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

#include <set>
#include <QtCore/QMap>

class QStringList;

enum class PluginState;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginStateStorage09
 * @short Loads states of plugin from 0.9 format of configuration.
 * @todo Split into two classes which can be unit-tested.
 *
 * All reads are done using global configuration API.
 */
class KADUAPI PluginStateStorage09
{

public:
	/**
	 * @param installedPluginNames list of known installed plugins
	 * @return Plugin states loaded from 0.9 format of configuration.
	 *
	 * If configuration in 0.9 format is not available, empty map is returned.
	 */
	QMap<QString, PluginState> load(const ::std::set<QString> &installedPluginNames) const;

};

/**
 * @}
 */
