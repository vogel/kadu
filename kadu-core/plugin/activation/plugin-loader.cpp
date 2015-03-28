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

#include "core/application.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/plugin-root-component.h"
#include "debug.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>

#ifdef Q_OS_MAC
	#define SO_PREFIX "lib"
	#define SO_EXT "dylib"
#elif defined(Q_OS_WIN)
	#define SO_PREFIX "lib"
	#define SO_EXT "dll"
#else
	#define SO_PREFIX "lib"
	#define SO_EXT "so"
#endif

PluginLoader::PluginLoader(const QString &pluginName, QObject *parent) noexcept(false) :
		// using C++ initializers breaks Qt's lupdate
		QObject(parent),
		m_pluginLoader(make_unique<QPluginLoader>(Application::instance()->pathsProvider()->pluginsLibPath() + "/" + QLatin1String(SO_PREFIX) + pluginName + QLatin1String("." SO_EXT)))
{
	m_pluginLoader->setLoadHints(QLibrary::ExportExternalSymbolsHint);

	if (!m_pluginLoader->load())
	{
		QString errorString = m_pluginLoader->errorString();
		kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(pluginName), qPrintable(errorString));

		throw PluginActivationErrorException(pluginName, tr("Cannot load %1 plugin library:\n%2").arg(pluginName, errorString));
	}
}

PluginLoader::~PluginLoader() noexcept
{
	if (m_pluginLoader)
	{
		// We need this because plugins can call deleteLater() just before being
		// unloaded. In this case control would not return to the event loop before
		// unloading the plugin and the event loop would try to delete objects
		// belonging to already unloaded plugins, which can result in segfaults.

		QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

		// probably we don't really need to unload plugins
		// root component gets its done() method called anyways, need to check
		// multiple loads/unloads if any problems arise
		// unloading plugins has some problems and disabling it is very easy way
		// to fix them
		// for example: if plugin after load adds some static data to glib,
		// like messaging-menu used in indicator-docking does, then after unload
		// and next load we are in trouble - application crashes
		// anyway, I don't expect users to unload plugins very frequently
		// m_pluginLoader->unload();
	}
}

PluginRootComponent * PluginLoader::instance() const noexcept
{
	if (!m_pluginLoader)
		return nullptr;

	return qobject_cast<PluginRootComponent *>(m_pluginLoader->instance());
}

#include "moc_plugin-loader.cpp"
