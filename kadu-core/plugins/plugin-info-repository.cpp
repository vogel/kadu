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

#include "plugin-info-repository.h"

#include "misc/algorithm.h"

PluginInfo PluginInfoRepository::converter(PluginInfoRepository::WrappedIterator iterator)
{
	return iterator->second;
}

PluginInfoRepository::PluginInfoRepository(QObject *parent) :
		QObject{parent}
{
}

PluginInfoRepository::~PluginInfoRepository()
{
}

PluginInfoRepository::Iterator PluginInfoRepository::begin()
{
	return Iterator{m_pluginInfos.begin(), converter};
}

PluginInfoRepository::Iterator PluginInfoRepository::end()
{
	return Iterator{m_pluginInfos.end(), converter};
}

void PluginInfoRepository::setPluginInfos(std::map<QString, PluginInfo> &&pluginInfos)
{
	m_pluginInfos = std::move(pluginInfos);
}

void PluginInfoRepository::addPluginInfo(const QString &name, PluginInfo pluginInfo)
{
	if (hasPluginInfo(name))
		return;

	m_pluginInfos.insert(std::make_pair(name, std::move(pluginInfo)));
}

void PluginInfoRepository::removePluginInfo(const QString &name)
{
	if (!hasPluginInfo(name))
		return;

	m_pluginInfos.erase(name);
}

bool PluginInfoRepository::hasPluginInfo(const QString &name) const
{
	return contains(m_pluginInfos, name);
}

PluginInfo PluginInfoRepository::pluginInfo(const QString &name) const
{
	return m_pluginInfos.at(name);
}

#include "moc_plugin-info-repository.cpp"
