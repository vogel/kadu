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

#pragma once

#include "plugins/plugin-loader.h"
#include "plugins/plugin-root-component-handler.h"
#include "plugins/plugin-translations-loader.h"

#include <QtCore/QScopedPointer>

class ActivePlugin
{

public:
	explicit ActivePlugin(const QString &pluginName, bool firstLoad);

private:
	// translations must be loaded first and uloaded last, see #2177
	QScopedPointer<PluginTranslationsLoader> m_pluginTranslationsLoader;
	QScopedPointer<PluginLoader> m_pluginLoader;
	QScopedPointer<PluginRootComponentHandler> m_pluginRootComponentHandler;

};
