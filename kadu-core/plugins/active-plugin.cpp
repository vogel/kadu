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

#include "active-plugin.h"

#include "configuration/configuration-file.h"
#include "misc/kadu-paths.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin.h"
#include "debug.h"

#include <QtCore/QEvent>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#ifdef Q_OS_MAC
	#define SO_PREFIX "lib"
	#define SO_EXT "dylib"
#elif defined(Q_OS_WIN)
	#define SO_PREFIX ""
	#define SO_EXT "dll"
#else
	#define SO_PREFIX "lib"
	#define SO_EXT "so"
#endif

ActivePlugin::ActivePlugin(Plugin *plugin, bool firstLoad, QObject *parent)
		: QObject{parent}, m_plugin{plugin}, m_pluginLoader{nullptr}, m_pluginObject{nullptr}, m_translator{nullptr}
{
	m_pluginLoader = new QPluginLoader(KaduPaths::instance()->pluginsLibPath() + "/" + QLatin1String(SO_PREFIX) + m_plugin->name() + QLatin1String("." SO_EXT));
	m_pluginLoader->setLoadHints(QLibrary::ExportExternalSymbolsHint);

	if (!m_pluginLoader->load())
	{
		QString err = m_pluginLoader->errorString();
		kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(m_plugin->name()), qPrintable(err));

		delete m_pluginLoader;
		m_pluginLoader = nullptr;

		kdebugf2();
		throw PluginActivationErrorException(plugin, tr("Cannot load %1 plugin library:\n%2").arg(m_plugin->name(), err));
	}

	// Load translations before the root component of the plugin is instantiated (it is done by instance() method).
	loadTranslations();

	m_pluginObject = qobject_cast<GenericPlugin *>(m_pluginLoader->instance());
	if (!m_pluginObject)
	{
		// Refer to deactivate() method for reasons to this.
		QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		m_pluginLoader->unload();
		delete m_pluginLoader;
		m_pluginLoader = nullptr;

		unloadTranslations();

		kdebugf2();
		throw PluginActivationErrorException(plugin, tr("Cannot find required object in module %1.\nMaybe it's not Kadu-compatible plugin.").arg(m_plugin->name()));
	}

	auto res = m_pluginObject->init(firstLoad);

	if (res != 0)
	{
		// Refer to deactivate() method for reasons to this.
		QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		m_pluginLoader->unload();
		delete m_pluginLoader;
		m_pluginLoader = nullptr;
		m_pluginObject = nullptr;

		unloadTranslations();

		throw PluginActivationErrorException(plugin, tr("Module initialization routine for %1 failed.").arg(m_plugin->name()));
	}
}

ActivePlugin::~ActivePlugin()
{
	if (m_pluginObject)
		m_pluginObject->done();

	// We need this because plugins can call deleteLater() just before being
	// unloaded. In this case control would not return to the event loop before
	// unloading the plugin and the event loop would try to delete objects
	// belonging to already unloaded plugins, which can result in segfaults.
	QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

	if (m_pluginLoader)
	{
		m_pluginLoader->unload();
		m_pluginLoader->deleteLater();
		m_pluginLoader = nullptr;
	}
	m_pluginObject = nullptr;

	// We cannot unload translations before calling PluginObject->done(), see #2177.
	unloadTranslations();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Loads translations for plugin.
 *
 * This method loads translations for current Kadu's language. Translations are loaded from
 * dataDir/kadu/plugins/translations/pluginName_lang.qm file.
 */
void ActivePlugin::loadTranslations()
{
	m_translator = new QTranslator{this};
	auto const lang = config_file.readEntry("General", "Language");

	if (m_translator->load(m_plugin->name() + '_' + lang, KaduPaths::instance()->dataPath() + QLatin1String("plugins/translations")))
		qApp->installTranslator(m_translator);
	else
	{
		delete m_translator;
		m_translator = nullptr;
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @author Bartosz 'beevvy' Brachaczek
 * @short Unloads translations for plugin.
 *
 * This method unloads existing translations for this plugin. Translations are removed
 * from QApplication instance and \c Translator is deleted.
 */
void ActivePlugin::unloadTranslations()
{
	if (m_translator)
	{
		qApp->removeTranslator(m_translator);
		delete m_translator;
		m_translator = nullptr;
	}
}

#include "moc_active-plugin.cpp"
