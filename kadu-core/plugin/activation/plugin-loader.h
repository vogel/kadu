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

#include <QtCore/QObject>
#include <memory>

class QPluginLoader;

class PluginRootComponent;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginLoader
 * @short RAII class for loading plugin dynamic library file.
 * @note This class uses hard-coded path for dynamic library file.
 *
 * This class loads plugin dynamic library file for given plugin name. In destructor library file is
 * unloaded.
 *
 * Root object of plugin is available by instance method.
 */
class PluginLoader : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Load plugin dynamic library file.
	 * @param pluginName name of plugin to load
	 * @param parent Qt parent object
	 * @throws PluginActivationErrorException
	 *
	 * Load plugin dynamic library file. In case load fails an PluginActivationErrorException exception is thrown.
	 */
	explicit PluginLoader(const QString &pluginName, QObject *parent = nullptr) noexcept(false);

	/**
	 * @short Unload plugin dynamic library file.
	 */
	virtual ~PluginLoader() noexcept;

	/**
	 * @return root object of plugin.
	 */
	PluginRootComponent * instance() const noexcept;

private:
	std::unique_ptr<QPluginLoader> m_pluginLoader;

};

/**
 * @}
 */
