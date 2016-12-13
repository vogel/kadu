/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "actions/injector-registered-actions.h"
#include "misc/memory.h"
#include "plugin/activation/plugin-injector-provider-container.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injector.h>
#include <memory>

class QPluginLoader;

class PluginInjectorProvider;

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
	 * @param pluginDirPath path of directory containing plugins
	 * @param pluginName name of plugin to load
	 * @param pluginInjectorProvider provider for already loaded injectors
	 * @param parent Qt parent object
	 * @throws PluginActivationErrorException
	 *
	 * Load plugin dynamic library file. In case load fails an PluginActivationErrorException exception is thrown.
	 */
	explicit PluginLoader(const QString &pluginDirPath, const QString &pluginName, PluginInjectorProvider *pluginInjectorProvider, QObject *parent = nullptr) noexcept(false);

	/**
	 * @short Unload plugin dynamic library file.
	 */
	virtual ~PluginLoader() noexcept;

	/**
	 * @return Injector of plugin.
	 */
	injeqt::injector & injector() const noexcept;

	/**
	 * @brief Starts plugin objects.
	 */
	void start();

private:
	std::unique_ptr<QPluginLoader> m_pluginLoader;
	mutable injeqt::injector m_pluginInjector;
	PluginInjectorProviderContainer m_pluginInjectorProviderContainer;
	InjectorRegisteredActions m_injectorRegisteredActions;

	std::unique_ptr<QPluginLoader> createPluginLoader(const QString &pluginDirPath, const QString &pluginName) const;
	injeqt::injector createPluginInjector(const QString &pluginName, PluginInjectorProvider *pluginInjectorProvider);

};

/**
 * @}
 */
