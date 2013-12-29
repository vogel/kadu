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
#include "plugins/plugin-info.h"
#include "exports.h"

#include <QtCore/QObject>
#include <map>

class PluginInfo;

class KADUAPI PluginInfoRepository : public QObject
{
	Q_OBJECT

	using Storage = ::std::map<QString, PluginInfo>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, PluginInfo>;

	explicit PluginInfoRepository(QObject *parent = nullptr);
	virtual ~PluginInfoRepository();

	Iterator begin();
	Iterator end();

	void addPluginInfo(const QString &name, PluginInfo pluginInfo);
	void removePluginInfo(const QString &name);

	bool hasPluginInfo(const QString &name) const;
	PluginInfo pluginInfo(const QString &name) const;

private:
	static PluginInfo converter(WrappedIterator iterator);

	std::map<QString, PluginInfo> m_pluginInfos;

};

inline PluginInfoRepository::Iterator begin(PluginInfoRepository *pluginRepository)
{
	return pluginRepository->begin();
}

inline PluginInfoRepository::Iterator end(PluginInfoRepository *pluginRepository)
{
	return pluginRepository->end();
}

/**
 * @}
 */
