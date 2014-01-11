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

#include "plugin-metadata-repository.h"

#include "misc/algorithm.h"

PluginMetadata PluginMetadataRepository::converter(PluginMetadataRepository::WrappedIterator iterator)
{
	return iterator->second;
}

PluginMetadataRepository::PluginMetadataRepository(QObject *parent) :
		QObject{parent}
{
}

PluginMetadataRepository::~PluginMetadataRepository()
{
}

PluginMetadataRepository::Iterator PluginMetadataRepository::begin()
{
	return Iterator{m_pluginMetadatas.begin(), converter};
}

PluginMetadataRepository::Iterator PluginMetadataRepository::end()
{
	return Iterator{m_pluginMetadatas.end(), converter};
}

void PluginMetadataRepository::setAllPluginMetadata(std::map<QString, PluginMetadata> &&pluginMetadatas)
{
	m_pluginMetadatas = std::move(pluginMetadatas);
}

void PluginMetadataRepository::addPluginMetadata(const QString &name, PluginMetadata pluginMetadata)
{
	if (hasPluginMetadata(name))
		return;

	m_pluginMetadatas.insert(std::make_pair(name, std::move(pluginMetadata)));
}

void PluginMetadataRepository::removePluginMetadata(const QString &name)
{
	if (!hasPluginMetadata(name))
		return;

	m_pluginMetadatas.erase(name);
}

const ::std::map<QString, PluginMetadata> & PluginMetadataRepository::allPluginMetadata() const
{
	return m_pluginMetadatas;
}

bool PluginMetadataRepository::hasPluginMetadata(const QString &name) const
{
	return contains(m_pluginMetadatas, name);
}

PluginMetadata PluginMetadataRepository::pluginMetadata(const QString &name) const
{
	return m_pluginMetadatas.at(name);
}

#include "moc_plugin-metadata-repository.cpp"
