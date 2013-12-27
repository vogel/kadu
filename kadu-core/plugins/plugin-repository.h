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
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>

class Plugin;

class KADUAPI PluginRepository : public QObject
{
	Q_OBJECT

	using Storage = QMap<QString, Plugin *>;
	using WrappedIterator = Storage::Iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, Plugin *>;

	explicit PluginRepository(QObject *parent = nullptr);
	virtual ~PluginRepository();

	Iterator begin();
	Iterator end();

	void addPlugin(const QString &name, Plugin *plugin);
	void removePlugin(const QString &name);

	bool hasPlugin(const QString &name) const;
	Plugin * plugin(const QString &name) const;

private:
	static Plugin * converter(WrappedIterator iterator);

	QMap<QString, Plugin *> m_plugins;

};

inline PluginRepository::Iterator begin(PluginRepository *pluginRepository)
{
	return pluginRepository->begin();
}

inline PluginRepository::Iterator end(PluginRepository *pluginRepository)
{
	return pluginRepository->end();
}

/**
 * @}
 */
