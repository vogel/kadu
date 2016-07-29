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

#include "active-plugin.h"

#include "misc/memory.h"

ActivePlugin::ActivePlugin(const QString &pluginDirPath, const QString &pluginTranslationDir, const QString &language, const QString &pluginName, PluginInjectorProvider *pluginInjectorProvider) :
		m_pluginTranslationsLoader{make_unique<PluginTranslationsLoader>(pluginTranslationDir, language, pluginName)},
		m_pluginLoader{make_unique<PluginLoader>(pluginDirPath, pluginName, pluginInjectorProvider)}
{
}

injeqt::injector & ActivePlugin::injector() const
{
	return m_pluginLoader->injector();
}

void ActivePlugin::start()
{
	return m_pluginLoader->start();
}
