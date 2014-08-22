/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin/activation/plugin-loader.h"
#include "plugin/activation/plugin-root-component-handler.h"
#include "plugin/activation/plugin-translations-loader.h"

#include <QtCore/QScopedPointer>
#include <memory>

class PluginRootComponent;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class ActivePlugin
 * @short Plugin that is loaded into memory and active.
 *
 * ActivePlugin loads plugin with translation in construtor and handles its root object (derived
 * from PluginRootComponent). Paths of translation file and plugin dynamic library are
 * currently hardcoded.
 *
 * All plugin data is loaded in constructor and plugin's root object is initialized (by calling its
 * init method). When ActivePlugin is destructed plugin's root object is deinitialized (by calling its
 * done method), then dynamic library is unloaded and translations are removed from application.
 *
 * PluginActivationErrorException can be thrown during object construction. All loaded data
 * is then removed from memory and root object is destroyed if neccessary.
 *
 * @note Translations must be loaded before root object is created and destroyed after its destruction.
 */
class ActivePlugin
{

public:
	/**
	 * @short Loads all plugin data.
	 * @throws PluginActivationErrorException
	 *
	 * Loads plugin transaltion for pluginName, then plugin library file. After that plugin root
	 * object is created and initialized. Parameter firstLoad is passed to plugin root object
	 * init method.
	 *
	 * In case of any error PluginActivationErrorException is thrown. All allocated data is properly
	 * destroyed and memory is freed in this case.
	 */
	explicit ActivePlugin(const QString &pluginName, bool firstLoad);

	/**
	 * @return Root component of plugin.
	 */
	PluginRootComponent * pluginRootComponent() const;

private:
	// translations must be loaded first and uloaded last, see #2177
	std::unique_ptr<PluginTranslationsLoader> m_pluginTranslationsLoader;
	std::unique_ptr<PluginLoader> m_pluginLoader;
	std::unique_ptr<PluginRootComponentHandler> m_pluginRootComponentHandler;

};

/**
 * @}
 */
