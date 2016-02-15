/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin-loader.h"

#include "misc/memory.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/plugin-injector-provider.h"
#include "plugin/plugin-modules-factory.h"
#include "injeqt-type-roles.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>

#if defined(Q_OS_WIN)
	#define SO_PREFIX "lib"
	#define SO_EXT "dll"
#else
	#define SO_PREFIX "lib"
	#define SO_EXT "so"
#endif

PluginLoader::PluginLoader(const QString &pluginDirPath, const QString &pluginName, PluginInjectorProvider *pluginInjectorProvider, QObject *parent) noexcept(false) :
		// using C++ initializers breaks Qt's lupdate
		QObject(parent),
		m_pluginLoader{createPluginLoader(pluginDirPath, pluginName)},
		m_pluginInjector{createPluginInjector(pluginName, pluginInjectorProvider)}
{
	m_pluginInjector.instantiate_all_with_type_role(PLUGIN);
	m_pluginInjector.instantiate_all_with_type_role(SERVICE);
}

PluginLoader::~PluginLoader() noexcept
{
	// do not unload plugin here
	// if plugin after load adds some static data to glib,
	// like messaging-menu used in indicator-docking does, then after unload
	// and next load we are in trouble - application crashes
	// anyway, I don't expect users to unload plugins very frequently
	// m_pluginLoader->unload();
}

injeqt::injector & PluginLoader::injector() const noexcept
{
	return m_pluginInjector;
}

std::unique_ptr<QPluginLoader> PluginLoader::createPluginLoader(const QString &pluginDirPath, const QString &pluginName) const
{
	auto result = make_unique<QPluginLoader>(pluginDirPath + "/" + QStringLiteral(SO_PREFIX) + pluginName + QStringLiteral("." SO_EXT));
	result->setLoadHints(QLibrary::ExportExternalSymbolsHint);
	return result;
}

injeqt::injector PluginLoader::createPluginInjector(const QString &pluginName, PluginInjectorProvider *pluginInjectorProvider)
{
	try
	{
		if (auto pluginModulesFactory = qobject_cast<PluginModulesFactory *>(m_pluginLoader->instance()))
		{
			auto parentInjectorName = pluginModulesFactory->parentInjectorName();
			auto parentInjector = &pluginInjectorProvider->injector(parentInjectorName);
			return injeqt::injector{std::vector<injeqt::injector *>{parentInjector}, pluginModulesFactory->createPluginModules()};
		}
		else
			return injeqt::injector{};
	}
	catch  (injeqt::exception::exception &e)
	{
		throw PluginActivationErrorException{pluginName, tr("Creating plugin injector for %1 failed.\n%2: %3").arg(pluginName).arg(typeid(e).name()).arg(e.what())};
	}
}

#include "moc_plugin-loader.cpp"
