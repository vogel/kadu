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
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-loader.h"
#include "plugins/plugin-root-component.h"
#include "plugins/plugin.h"
#include "debug.h"

#include <QtCore/QTranslator>
#include <QtGui/QApplication>

ActivePlugin::ActivePlugin(Plugin *plugin, bool firstLoad, QObject *parent)
		: QObject{parent}, m_plugin{plugin}, m_pluginLoader{nullptr}, m_pluginRootComponent{nullptr}, m_translator{nullptr}
{
	m_pluginLoader.reset(new PluginLoader(plugin));

	// Load translations before the root component of the plugin is instantiated (it is done by instance() method).
	loadTranslations();

	m_pluginRootComponent = m_pluginLoader->instance();
	if (!m_pluginRootComponent)
	{
		unloadTranslations();

		kdebugf2();
		throw PluginActivationErrorException(plugin, tr("Cannot find required object in module %1.\nMaybe it's not Kadu-compatible plugin.").arg(m_plugin->name()));
	}

	auto res = m_pluginRootComponent->init(firstLoad);

	if (res != 0)
	{
		m_pluginRootComponent = nullptr;

		unloadTranslations();

		throw PluginActivationErrorException(plugin, tr("Module initialization routine for %1 failed.").arg(m_plugin->name()));
	}
}

ActivePlugin::~ActivePlugin()
{
	if (m_pluginRootComponent)
		m_pluginRootComponent->done();

	m_pluginRootComponent = nullptr;

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
