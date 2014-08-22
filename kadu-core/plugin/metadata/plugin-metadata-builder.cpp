/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-metadata-builder.h"

#include "plugin/metadata/plugin-metadata.h"

PluginMetadataBuilder::PluginMetadataBuilder() :
		m_loadByDefault{true}
{
}

PluginMetadataBuilder & PluginMetadataBuilder::setName(const QString &name)
{
	m_name = name;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setDisplayName(const QString &displayName)
{
	m_displayName = displayName;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setCategory(const QString &category)
{
	m_category = category;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setType(const QString &type)
{
	m_type = type;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setDescription(const QString &description)
{
	m_description = description;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setAuthor(const QString &author)
{
	m_author = author;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setVersion(const QString &version)
{
	m_version = version;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setProvides(const QString &provides)
{
	m_provides = provides;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setDependencies(const QStringList &dependencies)
{
	m_dependencies = dependencies;
	return *this;
}

PluginMetadataBuilder & PluginMetadataBuilder::setReplaces(const QStringList &replaces)
{
	m_replaces = replaces;
	return *this;
}

PluginMetadataBuilder &PluginMetadataBuilder::setLoadByDefault(bool loadByDefault)
{
	m_loadByDefault = loadByDefault;
	return *this;
}

PluginMetadata PluginMetadataBuilder::create()
{
	return
	{
		m_name,
		m_displayName,
		m_category,
		m_type,
		m_description,
		m_author,
		m_version,
		m_provides,
		m_dependencies,
		m_replaces,
		m_loadByDefault
	};
}
