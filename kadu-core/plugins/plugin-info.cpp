/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "core/core.h"

#include "plugin-info.h"

PluginInfo::PluginInfo(const QString &fileName) :
		LoadByDefault(false)
{
	PlainConfigFile file(fileName);

	const QString lang = config_file.readEntry("General", "Language");

	Type = file.readEntry("Module", "Type");
	Description = file.readEntry("Module", "Description[" + lang + ']');
	if (Description.isEmpty())
		Description = file.readEntry("Module", "Description");

	Author = file.readEntry("Module", "Author");

	if (file.readEntry("Module", "Version") == "core")
		Version = Core::version();
	else
		Version = file.readEntry("Module", "Version");

	Dependencies = file.readEntry("Module", "Dependencies").split(' ', QString::SkipEmptyParts);
	Conflicts = file.readEntry("Module", "Conflicts").split(' ', QString::SkipEmptyParts);
	Provides = file.readEntry("Module", "Provides").split(' ', QString::SkipEmptyParts);
	Replaces = file.readEntry("Module", "Replaces").split(' ', QString::SkipEmptyParts);

	LoadByDefault = file.readBoolEntry("Module", "LoadByDefault");
}

PluginInfo::~PluginInfo()
{
}
