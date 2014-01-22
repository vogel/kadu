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

#include "plugin-metadata.h"

PluginMetadata::PluginMetadata() :
		m_loadByDefault{false}
{
}

PluginMetadata::PluginMetadata(
			QString name, QString displayName, QString category, QString type, QString description, QString author, QString version,
			QString provides, QStringList dependencies, QStringList replaces, bool loadByDefault) :
		m_name{std::move(name)}, m_displayName{std::move(displayName)}, m_category{std::move(category)}, m_type{std::move(type)},
		m_description{std::move(description)}, m_author{std::move(author)}, m_version{std::move(version)}, m_provides{std::move(provides)},
		m_dependencies{std::move(dependencies)}, m_replaces{std::move(replaces)}, m_loadByDefault{loadByDefault}
{
}

PluginMetadata::~PluginMetadata()
{
}

QString PluginMetadata::name() const
{
	return m_name;
}

QString PluginMetadata::displayName() const
{
	return m_displayName;
}

QString PluginMetadata::category() const
{
	return m_category;
}

QString PluginMetadata::type() const
{
	return m_type;
}

QString PluginMetadata::description() const
{
	return m_description;
}

QString PluginMetadata::author() const
{
	return m_author;
}

QString PluginMetadata::version() const
{
	return m_version;
}

QString PluginMetadata::provides() const
{
	return m_provides;
}

QStringList PluginMetadata::dependencies() const
{
	return m_dependencies;
}

QStringList PluginMetadata::replaces() const
{
	return m_replaces;
}

bool PluginMetadata::loadByDefault() const
{
	return m_loadByDefault;
}
