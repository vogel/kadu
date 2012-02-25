/**
  * This file is part of the KDE project
  * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
  * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License version 2 as published by the Free Software Foundation.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#ifndef PLUGIN_LIST_VIEW_H
#define PLUGIN_LIST_VIEW_H

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
class PluginListViewDelegate;
class Plugin;


/**
  * @short A widget to select what plugins to load and configure the plugins.
  *
  * It shows the list of available plugins
  *
  * Since the user needs a way to know what a specific plugin does every plugin
  * sould install a desktop file containing a name, comment and category field.
  * The category is useful for applications that can use different kinds of
  * plugins like a playlist, skin or visualization
  *
  * The location of these desktop files is the
  * share/apps/&lt;instancename&gt;/&lt;plugindir&gt; directory. But if you need
  * you may use a different directory
  *
  * You can add plugins from different KConfig[group], by just calling all times
  * you want addPlugins method with the correct parameters
  *
  * Additionally, calls to constructor with same @p categoryName, will add new
  * items to the same category, even if plugins are from different categories
  *
  * @author Matthias Kretz <kretz@kde.org>
  * @author Rafael Fernández López <ereslibre@kde.org>
  */

class PluginListView : public QWidget
{
        Q_OBJECT

public:
        enum PluginLoadMethod
        {
                ReadConfigFile = 0,
                IgnoreConfigFile
        };

        /**
          * Create a new PluginListView
          */
        PluginListView(QWidget *parent = 0);

        /**
          * Destructor
          */
        ~PluginListView();

Q_SIGNALS:
        /**
          * Tells you whether the configuration is changed or not.
          */
        void changed(bool hasChanged);

        /**
          * Emitted after the config of an embedded KCM has been saved. The
          * argument is the name of the parent component that needs to reload
          * its config
          */
        void configCommitted(const QByteArray &componentName);

private:

        class Private;
        Private * const d;
};

class PluginListView::Private
                        : public QObject
{
        Q_OBJECT

public:
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

        Private(PluginListView *parent);
        ~Private();

        int dependantLayoutValue(int value, int width, int totalWidth) const;

public:

        class PluginModel;
        class ProxyModel;
        class PluginDelegate;
        QObject *parent;
        QLineEdit *lineEdit;
        CategorizedListView *listView;
        CategorizedListViewPainter *categoryDrawer;
        PluginModel *pluginModel;
        ProxyModel *proxyModel;
        PluginDelegate *pluginDelegate;
        bool showIcons;
};

class PluginEntry
{

public:
        QString category;
        QString name;
        QString description;
        bool checked;
        bool isCheckable;

        bool operator==(const PluginEntry &pe) const
        {
                return name == pe.name;
        }
};

Q_DECLARE_METATYPE(PluginEntry*)

class PluginListView::Private::PluginModel
                        : public QAbstractListModel
{
	Q_OBJECT

public:
        PluginModel(PluginListView::Private *pluginSelector_d, QObject *parent = 0);
        ~PluginModel();

        virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

        void loadPluginData();

private:
        PluginListView::Private *pluginSelector_d;
        PluginsManager *Manager;
        QList<PluginEntry> Plugins;

private slots:
	void pluginAdded(const Plugin *plugin);
	void pluginRemoved(const Plugin *plugin);
};

class PluginListView::Private::ProxyModel
                        : public CategorizedSortFilterProxyModel
{

public:
        ProxyModel(PluginListView::Private *pluginSelector_d, QObject *parent = 0);
        ~ProxyModel();

protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        virtual bool subSortLessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
        PluginListView::Private *pluginSelector_d;
};


class PluginListView::Private::PluginDelegate
                        : public PluginListViewDelegate
{
        Q_OBJECT

public:
        PluginDelegate(PluginListView::Private *pluginSelector_d, QObject *parent = 0);
        ~PluginDelegate();

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

Q_SIGNALS:
        void changed(bool hasChanged);
        void configCommitted(const QByteArray &componentName);

protected:
        virtual QList<QWidget*> createItemWidgets() const;
        virtual void updateItemWidgets(const QList<QWidget*> widgets,
                                       const QStyleOptionViewItem &option,
                                       const QPersistentModelIndex &index) const;

private Q_SLOTS:
        void slotStateChanged(bool state);
        void emitChanged();
        void slotAboutClicked();
        void slotConfigureClicked();

private:
        QFont titleFont(const QFont &baseFont) const;

        QCheckBox *checkBox;
        QPushButton *pushButton;

        PluginListView::Private *pluginSelector_d;
};



#endif
