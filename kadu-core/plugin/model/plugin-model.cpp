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

#include "plugin-model.h"

#include "model/categorized-sort-filter-proxy-model.h"
#include "plugin/metadata/plugin-metadata.h"

PluginModel::PluginModel(QObject *parent) :
		QAbstractListModel{parent}
{
}

PluginModel::~PluginModel()
{
}

void PluginModel::setPluginEntries(QVector<PluginMetadata> pluginEntries)
{
	beginResetModel();
	m_pluginEntries = std::move(pluginEntries);
	endResetModel();
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

QModelIndex PluginModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return createIndex(row, column, (row < m_pluginEntries.count()) ? row : 0);
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_pluginEntries.count())
		return {};

	auto pluginMetadata = m_pluginEntries.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
			return !pluginMetadata.displayName().isEmpty() ? pluginMetadata.displayName() : pluginMetadata.name();
		case MetadataRole:
			return QVariant::fromValue(pluginMetadata);
		case NameRole:
			return pluginMetadata.name();
		case CommentRole:
			return pluginMetadata.description();
		case Qt::CheckStateRole:
			return m_activePlugins.contains(pluginMetadata.name());
		case CategorizedSortFilterProxyModel::CategoryDisplayRole:
		case CategorizedSortFilterProxyModel::CategorySortRole:
			return !pluginMetadata.category().isEmpty() ? pluginMetadata.category() : "Misc";
		default:
			return {};
	}
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && (role == Qt::CheckStateRole))
	{
		auto pluginName = index.data(NameRole).toString();
		if (value.toBool())
			m_activePlugins.insert(pluginName);
		else
			m_activePlugins.remove(pluginName);
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
