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

#include <memory>

class QString;
class QTranslator;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginTranslationsLoader
 * @short RAII class for handling plugin translation.
 * @note This class uses hard-coded path for translation file.
 */
class PluginTranslationsLoader
{

public:
	/**
	 * @short Load plugin translation file.
	 * @param pluginName name of plugin to load
	 *
	 * Loads and installs plugin translation file in application. If for some reason it
	 * was impossible - this constructor does nothing.
	 */
	explicit PluginTranslationsLoader(const QString &pluginName) noexcept;

	/**
	 * @short Unload plugin translation file.
	 */
	~PluginTranslationsLoader() noexcept;

private:
	std::unique_ptr<QTranslator> m_translator;

};

/**
 * @}
 */
