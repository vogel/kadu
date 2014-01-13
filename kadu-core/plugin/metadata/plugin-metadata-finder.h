/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <map>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class PluginMetadata;
class PluginMetadataReader;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginMetadataFinder
 * @short Finds metadata .desc files and returns @see PluginMetadata instances from them.
 *
 * This class searchs directory for .desc files and reads metadata from them.
 */
class PluginMetadataFinder : public QObject
{
	Q_OBJECT

public:
	explicit PluginMetadataFinder(QObject *parent = nullptr) noexcept;
	virtual ~PluginMetadataFinder() noexcept;

	/**
	 * @short Set PluginMetadataReader service instance.
	 *
	 * @see PluginMetadataReader is used to read metadata from .desc files.
	 */
	void setPluginMetadataReader(PluginMetadataReader *pluginMetadataReader) noexcept;

	/**
	 * @short Read metadata files from given directory.
	 * @param directory directory to search for metadata files
	 *
	 * Scans \p directory for metadata files and reads. Result is a map of file name (that is also
	 * a plugin name) to @see PluginMetadata objects read from these files.
	 */
	std::map<QString, PluginMetadata> readAllPluginMetadata(const QString &directory) noexcept;

private:
	QPointer<PluginMetadataReader> m_pluginMetadataReader;

};

/**
 * @}
 */
