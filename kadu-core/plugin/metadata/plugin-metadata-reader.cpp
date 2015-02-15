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

#include "plugin-metadata-reader.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "plugin/metadata/plugin-metadata-builder.h"
#include "plugin/metadata/plugin-metadata-reader-exception.h"
#include "plugin/metadata/plugin-metadata.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

PluginMetadataReader::PluginMetadataReader(QObject *parent) noexcept :
		QObject(parent)
{
}

PluginMetadataReader::~PluginMetadataReader() noexcept
{
}

PluginMetadata PluginMetadataReader::readPluginMetadata(const QString &pluginName, const QString &filePath) noexcept(false)
{
	auto fileInfo = QFileInfo{filePath};
	if (!fileInfo.exists() || !fileInfo.isReadable())
		throw PluginMetadataReaderException{};

	auto const lang = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language");
	QSettings file{filePath, QSettings::IniFormat};
	file.setIniCodec("UTF-8");

	auto builder = PluginMetadataBuilder{};
	return builder
			.setName(pluginName)
			.setDisplayName(file.value("Module/DisplayName[" + lang + ']', file.value("Module/DisplayName")).toString())
			.setCategory(file.value("Module/Category").toString())
			.setType(file.value("Module/Type").toString())
			.setDescription(file.value("Module/Description[" + lang + ']', file.value("Module/Description")).toString())
			.setAuthor(file.value("Module/Author").toString())
			.setVersion(file.value("Module/Version").toString() == "core"
					? Core::version()
					: file.value("Module/Version").toString())
			.setProvides(file.value("Module/Provides").toString())
			.setDependencies(file.value("Module/Dependencies").toString().split(' ', QString::SkipEmptyParts))
			.setReplaces(file.value("Module/Replaces").toString().split(' ', QString::SkipEmptyParts))
			.setLoadByDefault(file.value("Module/LoadByDefault").toBool())
			.create();
}
