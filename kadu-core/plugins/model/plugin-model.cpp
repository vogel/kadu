/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#define MARGIN 5

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"
#include "plugins/model/plugin-proxy-model.h"

#include "plugin-model.h"

void PluginModel::loadPluginData()
{
        beginResetModel();
        Plugins.clear();
        QList<PluginEntry> listToAdd;

        foreach (Plugin *plugin, PluginsManager::instance()->plugins())
        {
                PluginEntry pluginEntry;
                PluginInfo *pluginInfo = plugin->info();

                pluginEntry.category = pluginInfo && !pluginInfo->category().isEmpty()
                        ? pluginInfo->category()
                        : "Misc";
                pluginEntry.name = pluginInfo && !pluginInfo->displayName().isEmpty()
                        ? pluginInfo->displayName()
                        : plugin->name();
                pluginEntry.description = pluginInfo
                        ? pluginInfo->description()
                        : QString();
                pluginEntry.author = pluginInfo
                        ? pluginInfo->author()
                        : QString();
                pluginEntry.pluginName = plugin->name();
                pluginEntry.checked = plugin->isActive();
                pluginEntry.isCheckable = true;

                listToAdd.append(pluginEntry);
        }

        Plugins << listToAdd;
        endResetModel();

        pluginSelector_d->Proxy->sort(0);
}

PluginModel::PluginModel(PluginListWidget *pluginSelector_d, QObject *parent)
                : QAbstractListModel(parent)
                , pluginSelector_d(pluginSelector_d)
                , Manager(PluginsManager::instance())
{
}

PluginModel::~PluginModel()
{
}

QModelIndex PluginModel::index(int row, int column, const QModelIndex &parent) const
{
        Q_UNUSED(parent)

        return createIndex(row, column, (row < Plugins.count()) ? (void*) &Plugins.at(row) : 0);
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
        if (!index.isValid() || !index.internalPointer())
        {
                return QVariant();
        }

        if (index.row() < 0 || index.row() >= Plugins.count())
                return QVariant();

        PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());


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

                case ServicesCountRole:

                case IsCheckableRole:
                        return true;

                case Qt::CheckStateRole:
                        return pluginEntry->checked;

                case CategorizedSortFilterProxyModel::CategoryDisplayRole: // fall through

                case CategorizedSortFilterProxyModel::CategorySortRole:
                        return pluginEntry->category;

                default:
                        return QVariant();
        }
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
        if (!index.isValid())
        {
                return false;
        }

        bool ret = false;

        if (role == Qt::CheckStateRole)
        {
                static_cast<PluginEntry*>(index.internalPointer())->checked = value.toBool();
                ret = true;
        }

        if (ret)
        {
                emit dataChanged(index, index);
        }

        return ret;
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
        return parent.isValid() ? 0 : Plugins.count();
}
