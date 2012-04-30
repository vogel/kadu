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

#ifndef PLUGIN_MODEL_H
#define PLUGIN_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtGui/QAbstractItemDelegate>
#include <QtGui/QWidget>

#include "gui/widgets/plugin-list-view-delegate.h"
#include "model/categorized-sort-filter-proxy-model.h"

class QCheckBox;
class QPushButton;
class QAbstractItemView;

class CategorizedListView;
class CategorizedListViewPainter;
// class FilterWidget;
class PluginsManager;
class PluginListWidgetDelegate;
class Plugin;
class PluginEntry;
class PluginListWidget;


class PluginModel : public QAbstractListModel
{
	Q_OBJECT

public:
        PluginModel(PluginListWidget *pluginSelector_d, QObject *parent = 0);
        ~PluginModel();

        enum ExtraRoles
        {
                PluginEntryRole   = 0x09386561,
                ServicesCountRole = 0x1422E2AA,
                NameRole          = 0x0CBBBB00,
                CommentRole       = 0x19FC6DE2,
                AuthorRole        = 0x30861E10,
                EmailRole         = 0x02BE3775,
                WebsiteRole       = 0x13095A34,
                VersionRole       = 0x0A0CB450,
                LicenseRole       = 0x001F308A,
                DependenciesRole  = 0x04CAB650,
                IsCheckableRole   = 0x0AC2AFF8
        };

        virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

        void loadPluginData();

private:
        PluginListWidget *pluginSelector_d;
        PluginsManager *Manager;
        QList<PluginEntry> Plugins;
};


class PluginEntry
{

public:
        QString category;
        QString name;
        QString pluginName;
        QString description;
        QString author;
        bool checked;
        bool isCheckable;

        bool operator==(const PluginEntry &pe) const
        {
                return name == pe.name;
        }
};

Q_DECLARE_METATYPE(PluginEntry*)


#endif