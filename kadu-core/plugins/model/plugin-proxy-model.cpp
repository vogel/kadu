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
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"
#include "plugins/model/plugin-model.h"

#include "plugin-proxy-model.h"


PluginProxyModel::PluginProxyModel(PluginListWidget *pluginSelector_d, QObject *parent)
                : CategorizedSortFilterProxyModel(parent)
                , pluginSelector_d(pluginSelector_d)
{
        sort(0);
}

PluginProxyModel::~PluginProxyModel()
{
}

bool PluginProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
        Q_UNUSED(sourceParent)

        if (!pluginSelector_d->LineEdit->filterText().isEmpty())
        {
                const QModelIndex index = sourceModel()->index(sourceRow, 0);
                const PluginEntry *entry = static_cast<PluginEntry*>(index.internalPointer());
                return entry->pluginName.contains(pluginSelector_d->LineEdit->filterText(), Qt::CaseInsensitive) ||
                       entry->name.contains(pluginSelector_d->LineEdit->filterText(), Qt::CaseInsensitive) ||
                       entry->description.contains(pluginSelector_d->LineEdit->filterText(), Qt::CaseInsensitive) ||
                       entry->author.contains(pluginSelector_d->LineEdit->filterText(), Qt::CaseInsensitive);
        }

        return true;
}

bool PluginProxyModel::subSortLessThan(const QModelIndex &left, const QModelIndex &right) const
{
        return static_cast<PluginEntry*>(left.internalPointer())->name.compare(static_cast<PluginEntry*>(right.internalPointer())->name, Qt::CaseInsensitive) < 0;
}
