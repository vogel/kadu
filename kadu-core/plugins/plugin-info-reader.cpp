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

#include "plugin-info-reader.h"

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "plugins/plugin-info-builder.h"
#include "plugins/plugin-info-reader-exception.h"
#include "plugins/plugin-info.h"

#include <QtCore/QFileInfo>

PluginInfoReader::PluginInfoReader(QObject *parent) noexcept :
		QObject(parent)
{
}

PluginInfoReader::~PluginInfoReader() noexcept
{
}

PluginInfo PluginInfoReader::readPluginInfo(QString name, const QString &filePath) noexcept(false)
{
	auto fileInfo = QFileInfo{filePath};
	if (!fileInfo.exists())
		throw PluginInfoReaderException();

	auto const lang = config_file.readEntry("General", "Language");
	PlainConfigFile file{filePath, "UTF-8"};

	auto builder = PluginInfoBuilder{};
	return builder
			.setName(name)
			.setDisplayName(file.readEntry("Module", "DisplayName[" + lang + ']', file.readEntry("Module", "DisplayName")))
			.setCategory(file.readEntry("Module", "Category"))
			.setType(file.readEntry("Module", "Type"))
			.setDescription(file.readEntry("Module", "Description[" + lang + ']', file.readEntry("Module", "Description")))
			.setAuthor(file.readEntry("Module", "Author"))
			.setVersion(file.readEntry("Module", "Version") == "core"
					? Core::version()
					: file.readEntry("Module", "Version"))
			.setProvides(file.readEntry("Module", "Provides"))
			.setDependencies(file.readEntry("Module", "Dependencies").split(' ', QString::SkipEmptyParts))
			.setReplaces(file.readEntry("Module", "Replaces").split(' ', QString::SkipEmptyParts))
			.setLoadByDefault(file.readBoolEntry("Module", "LoadByDefault"))
			.create();
}
