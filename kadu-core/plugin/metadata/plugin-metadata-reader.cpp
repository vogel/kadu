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
#include "core/version-service.h"
#include "plugin/metadata/plugin-metadata-reader-exception.h"
#include "plugin/metadata/plugin-metadata.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

PluginMetadataReader::PluginMetadataReader(QObject *parent) noexcept : QObject(parent)
{
}

PluginMetadataReader::~PluginMetadataReader() noexcept
{
}

void PluginMetadataReader::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void PluginMetadataReader::setVersionService(VersionService *versionService)
{
    m_versionService = versionService;
}

PluginMetadata
PluginMetadataReader::readPluginMetadata(const QString &pluginName, const QString &filePath) noexcept(false)
{
    auto fileInfo = QFileInfo{filePath};
    if (!fileInfo.exists() || !fileInfo.isReadable())
        throw PluginMetadataReaderException{};

    auto const lang = m_configuration->deprecatedApi()->readEntry("General", "Language");
    QSettings file{filePath, QSettings::IniFormat};
    file.setIniCodec("UTF-8");

    auto result = PluginMetadata{};
    result.name = pluginName;
    result.displayName = file.value("Module/DisplayName[" + lang + ']', file.value("Module/DisplayName")).toString();
    result.category = file.value("Module/Category").toString();
    result.type = file.value("Module/Type").toString();
    result.description = file.value("Module/Description[" + lang + ']', file.value("Module/Description")).toString();
    result.author = file.value("Module/Author").toString();
    result.version = file.value("Module/Version").toString() == "core" ? m_versionService->version()
                                                                       : file.value("Module/Version").toString();
    result.provides = file.value("Module/Provides").toString();
    result.dependencies = file.value("Module/Dependencies").toString().split(' ', QString::SkipEmptyParts);
    result.replaces = file.value("Module/Replaces").toString().split(' ', QString::SkipEmptyParts);
    result.loadByDefault = file.value("Module/LoadByDefault").toBool();
    result.internal = file.value("Module/Internal").toBool();

    return result;
}
