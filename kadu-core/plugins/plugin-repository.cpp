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

#include "plugin-repository.h"

Plugin * PluginRepository::converter(PluginRepository::WrappedIterator iterator)
{
	return *iterator;
}

PluginRepository::PluginRepository(QObject *parent) :
		QObject{parent}
{
}

PluginRepository::~PluginRepository()
{
}

PluginRepository::Iterator PluginRepository::begin()
{
	return Iterator{m_plugins.begin(), converter};
}

PluginRepository::Iterator PluginRepository::end()
{
	return Iterator{m_plugins.end(), converter};
}

void PluginRepository::addPlugin(const QString &name, Plugin *plugin)
{
	if (!plugin || hasPlugin(name))
		return;

	m_plugins.insert(name, plugin);
}

void PluginRepository::removePlugin(const QString &name)
{
	if (!hasPlugin(name))
		return;

	m_plugins.remove(name);
}

bool PluginRepository::hasPlugin(const QString &name) const
{
	return m_plugins.contains(name);
}

Plugin * PluginRepository::plugin(const QString &name) const
{
	return m_plugins.value(name);
}

#include "moc_plugin-repository.cpp"
