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

#include "misc/iterator.h"
#include "plugin/metadata/plugin-metadata.h"
#include "exports.h"

#include <map>
#include <set>
#include <QtCore/QObject>

class PluginMetadata;

class KADUAPI PluginMetadataRepository : public QObject
{
	Q_OBJECT

	using Storage = ::std::map<QString, PluginMetadata>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, PluginMetadata>;

	explicit PluginMetadataRepository(QObject *parent = nullptr);
	virtual ~PluginMetadataRepository();

	Iterator begin();
	Iterator end();

	void setAllPluginMetadata(std::map<QString, PluginMetadata> &&pluginMetadatas);
	void addPluginMetadata(const QString &name, PluginMetadata pluginMetadata);
	void removePluginMetadata(const QString &name);

	std::set<QString> pluginNames() const;
	const ::std::map<QString, PluginMetadata> & allPluginMetadata() const;

	bool hasPluginMetadata(const QString &name) const;
	PluginMetadata pluginMetadata(const QString &name) const;

private:
	static PluginMetadata converter(WrappedIterator iterator);

	std::map<QString, PluginMetadata> m_pluginMetadatas;

};

inline PluginMetadataRepository::Iterator begin(PluginMetadataRepository *pluginRepository)
{
	return pluginRepository->begin();
}

inline PluginMetadataRepository::Iterator end(PluginMetadataRepository *pluginRepository)
{
	return pluginRepository->end();
}

/**
 * @}
 */
