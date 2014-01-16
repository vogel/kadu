/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

class QString;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @enum PluginState
 * @short Plugin state.
 *
 * State of plugin selected by user. Plugin state is stored in configuration file and can be changed
 * in plugin configuration window.
 */
enum class PluginState
{
	/**
	 * Plugin has been never successfully activated before. May be activated automatically
	 * if has PluginMetadata::loadByDefault() set to true.
	 */
	New,

	/**
	 * Plugin is normally disabled and will be activated only as dependency of other plugin
	 * or at user request.
	 */
	Disabled,

	/**
	 * Plugin is enabled and will be activated at Kadu start automatically.
	 */
	Enabled

};

/**
 * @short Translate plugin state enum to string.
 */
KADUAPI QString pluginStateToString(PluginState pluginState);

/**
 * @short Translate string to plugin state enum.
 *
 * In case of invalid string returns PluginState::New.
 */
KADUAPI PluginState stringToPluginState(const QString &string);

/**
 * @}
 */
