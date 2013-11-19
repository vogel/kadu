/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-info.h"

#include "configuration/configuration-file.h"
#include "core/core.h"

PluginInfo::PluginInfo(const QString &fileName) :
		m_loadByDefault{false}
{
	PlainConfigFile file{fileName, "UTF-8"};
	auto const lang = config_file.readEntry("General", "Language");

	m_displayName = file.readEntry("Module", "DisplayName[" + lang + ']');
	if (m_displayName.isEmpty())
		m_displayName = file.readEntry("Module", "DisplayName");

	m_type = file.readEntry("Module", "Type");
	m_category = file.readEntry("Module", "Category");
	m_description = file.readEntry("Module", "Description[" + lang + ']');
	if (m_description.isEmpty())
		m_description = file.readEntry("Module", "Description");

	m_author = file.readEntry("Module", "Author");

	if (file.readEntry("Module", "Version") == "core")
		m_version = Core::version();
	else
		m_version = file.readEntry("Module", "Version");

	m_dependencies = file.readEntry("Module", "Dependencies").split(' ', QString::SkipEmptyParts);
	m_conflicts = file.readEntry("Module", "Conflicts").split(' ', QString::SkipEmptyParts);
	m_provides = file.readEntry("Module", "Provides").split(' ', QString::SkipEmptyParts);
	m_replaces = file.readEntry("Module", "Replaces").split(' ', QString::SkipEmptyParts);

	m_loadByDefault = file.readBoolEntry("Module", "LoadByDefault");
}

PluginInfo::~PluginInfo()
{
}

QString PluginInfo::type() const
{
	return m_type;
}

QStringList PluginInfo::dependencies() const
{
	return m_dependencies;
}

QStringList PluginInfo::conflicts() const
{
	return m_conflicts;
}

QStringList PluginInfo::provides() const
{
	return m_provides;
}

QStringList PluginInfo::replaces() const
{
	return m_replaces;
}

QString PluginInfo::description() const
{
	return m_description;
}

QString PluginInfo::author() const
{
	return m_author;
}

QString PluginInfo::version() const
{
	return m_version;
}

bool PluginInfo::loadByDefault() const
{
	return m_loadByDefault;
}

QString PluginInfo::category() const
{
	return m_category;
}

QString PluginInfo::displayName() const
{
	return m_displayName;
}
