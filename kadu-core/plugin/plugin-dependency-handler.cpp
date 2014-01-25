/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-dependency-handler.h"

#include "misc/algorithm.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/metadata/plugin-metadata-provider.h"

PluginMetadata PluginDependencyHandler::converter(WrappedIterator iterator)
{
	return iterator->second;
}

PluginDependencyHandler::PluginDependencyHandler(QObject *parent) :
		QObject(parent)
{
}

PluginDependencyHandler::~PluginDependencyHandler()
{
}

void PluginDependencyHandler::setPluginDependencyGraphBuilder(PluginDependencyGraphBuilder *pluginDependencyGraphBuilder)
{
	m_pluginDependencyGraphBuilder = pluginDependencyGraphBuilder;
}

void PluginDependencyHandler::setPluginMetadataProvider(PluginMetadataProvider *pluginMetadataProvider)
{
	m_pluginMetadataProvider = pluginMetadataProvider;
}

PluginDependencyHandler::Iterator PluginDependencyHandler::begin()
{
	return {m_allPluginMetadata.begin(), converter};
}

PluginDependencyHandler::Iterator PluginDependencyHandler::end()
{
	return {m_allPluginMetadata.end(), converter};
}

void PluginDependencyHandler::initialize()
{
	if (!m_pluginDependencyGraphBuilder || !m_pluginMetadataProvider)
		return;

	auto metadata = m_pluginMetadataProvider->provide();
	m_pluginDependencyDAG = m_pluginDependencyGraphBuilder->buildValidGraph(metadata);
	auto pluginsInDAG = m_pluginDependencyDAG.plugins(); // expensive method

	std::copy_if(metadata.begin(), metadata.end(), std::inserter(m_allPluginMetadata, m_allPluginMetadata.begin()),
		[&pluginsInDAG](const std::map<QString, PluginMetadata>::value_type &v){ return contains(pluginsInDAG, v.first); }
	);
}

std::set<QString> PluginDependencyHandler::pluginNames() const
{
	auto result = std::set<QString>{};
	std::transform(std::begin(m_allPluginMetadata), std::end(m_allPluginMetadata), std::inserter(result, result.begin()),
		[](const std::map<QString, PluginMetadata>::value_type &v){ return v.first; });
	return result;
}

bool PluginDependencyHandler::hasPluginMetadata(const QString &pluginName) const
{
	return contains(m_allPluginMetadata, pluginName);
}

PluginMetadata PluginDependencyHandler::pluginMetadata(const QString &pluginName) const
{
	return m_allPluginMetadata.at(pluginName);
}

QVector<QString> PluginDependencyHandler::withDependencies(const QString &pluginName) const
{
	return hasPluginMetadata(pluginName)
			? m_pluginDependencyDAG.findDependencies(pluginName) << pluginName
			: QVector<QString>{};
}

QVector<QString> PluginDependencyHandler::findDependencies(const QString &pluginName) const
{
	return hasPluginMetadata(pluginName)
			? m_pluginDependencyDAG.findDependencies(pluginName)
			: QVector<QString>{};
}

QVector<QString> PluginDependencyHandler::withDependents(const QString &pluginName) const
{
	return hasPluginMetadata(pluginName)
			? m_pluginDependencyDAG.findDependents(pluginName) << pluginName
			: QVector<QString>{};
}

QVector<QString> PluginDependencyHandler::findDependents(const QString &pluginName) const
{
	return hasPluginMetadata(pluginName)
			? m_pluginDependencyDAG.findDependents(pluginName)
			: QVector<QString>{};
}

#include "moc_plugin-dependency-handler.cpp"
