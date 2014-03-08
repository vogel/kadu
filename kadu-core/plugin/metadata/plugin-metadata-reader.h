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

#include <QtCore/QObject>

class PluginMetadata;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginMetadataReader
 * @short Reads PluginMetadata instance from .desc file.
 */
class PluginMetadataReader : public QObject
{
	Q_OBJECT

public:
	explicit PluginMetadataReader(QObject *parent = nullptr) noexcept;
	virtual ~PluginMetadataReader() noexcept;

	/**
	 * @short Reads PluginMetadata instance from .desc file.
	 * @param name name of plugin do read data
	 * @param filePath path to .desc file to read data from
	 * @throws PluginMetadataReaderException
	 *
	 * If file \p filePath can not be read, a PluginMetadataReaderException exception is thrown.
	 */
	PluginMetadata readPluginMetadata(const QString &pluginName, const QString &filePath) noexcept(false);

};

/**
 * @}
 */
