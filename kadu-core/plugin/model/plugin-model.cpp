/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#include "gui/widgets/plugin-list/plugin-list-widget.h"
#include "plugin/model/plugin-proxy-model.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/plugin-dependency-handler.h"

#include "plugin-model.h"

PluginModel::PluginModel(PluginListWidget *pluginListWidget, QObject *parent) :
		QAbstractListModel{parent}, m_pluginListWidget{pluginListWidget}
{
}

PluginModel::~PluginModel()
{
}

void PluginModel::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;
}

void PluginModel::setActivePlugins(QSet<QString> activePlugins)
{
	m_activePlugins = std::move(activePlugins);
	emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

const QSet<QString> & PluginModel::activePlugins() const
{
	return m_activePlugins;
}

void PluginModel::loadPluginData()
{
	if (!m_pluginDependencyHandler)
		return;

	beginResetModel();

	m_pluginEntries.clear();

	for (auto const &pluginMetadata : m_pluginDependencyHandler)
		m_pluginEntries.append({
			!pluginMetadata.category().isEmpty() ? pluginMetadata.category() : "Misc",
			!pluginMetadata.displayName().isEmpty() ? pluginMetadata.displayName() : pluginMetadata.name(),
			pluginMetadata.name(),
			pluginMetadata.description(),
			pluginMetadata.author()
		});

	endResetModel();

	m_pluginListWidget->Proxy->sort(0);
}

QModelIndex PluginModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return createIndex(row, column, (row < m_pluginEntries.count()) ? (void*) &m_pluginEntries.at(row) : nullptr);
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || !index.internalPointer())
		return {};

	if (index.row() < 0 || index.row() >= m_pluginEntries.count())
		return {};

	auto pluginEntry = static_cast<PluginEntry*>(index.internalPointer());

	switch (role)
	{
		case Qt::DisplayRole:
			return pluginEntry->name;
		case PluginEntryRole:
			return QVariant::fromValue(pluginEntry);
		case NameRole:
			return pluginEntry->pluginName;
		case CommentRole:
			return pluginEntry->description;
		case Qt::CheckStateRole:
			return m_activePlugins.contains(pluginEntry->pluginName);
		case CategorizedSortFilterProxyModel::CategoryDisplayRole:
		case CategorizedSortFilterProxyModel::CategorySortRole:
			return pluginEntry->category;
		default:
			return {};
	}
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && (role == Qt::CheckStateRole))
	{
		auto pluginEntry = static_cast<PluginEntry*>(index.internalPointer());
		if (value.toBool())
			m_activePlugins.insert(pluginEntry->pluginName);
		else
			m_activePlugins.remove(pluginEntry->pluginName);
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : m_pluginEntries.count();
}

#include "moc_plugin-model.cpp"
