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

#ifndef KPLUGINSELECTOR_H
#define KPLUGINSELECTOR_H

#include <QtGui/QWidget>

#include <QtCore/QList>


#include <QtCore/QAbstractListModel>
#include <QtGui/QAbstractItemDelegate>

// #include <kconfiggroup.h>
// #include <kplugininfo.h>
// #include "kwidgetitemdelegate.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "gui/widgets/plugin-list-view-delegate.h"


class QLabel;
class QCheckBox;
class QPushButton;
class QAbstractItemView;

class KLineEdit;
class CategorizedListView;
class KCModuleProxy;
class CategorizedListViewPainter;
class PluginsManager;
class PluginListViewDelegate;

class Plugin;

// class PluginEntry;



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
class PluginListView
    : public QWidget
{
    Q_OBJECT

public:
    enum PluginLoadMethod {
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

    /**
      * Add a list of KParts plugins
      *
      * The information about the plugins will be loaded from the
      * share/apps/&lt;instancename&gt;/kpartplugins directory
      *
      * @param componentName The name of the KComponentData of the plugin's parent.
      * @param categoryName  The translated name of the category. This is the
      *                      name that is shown in the title. If the category
      *                      did exist before because of another call to
      *                      addPlugins, then they will be shown in that
      *                      category. If @p categoryName is a new one, then
      *                      a new category will be shown on the plugin window,
      *                      and the list of plugins added to it
      * @param categoryKey   When you have different categories of KParts
      *                      plugins you distinguish between the plugins using
      *                      the Category key in the .desktop file. Use this
      *                      parameter to select only those KParts plugins
      *                      with the Category key == @p categoryKey. If
      *                      @p categoryKey is not set the Category key is
      *                      ignored and all plugins are shown. Not match case
      * @param config        The KConfig object that holds the state of the
      *                      plugins being enabled or not. By default it should
      *                      be componentData.config(). It is recommended to
      *                      always pass a KConfig object if you use
      *                      KSettings::PluginPage since you never know from where the
      *                      page will be called (think global config app).
      *                      For example KViewCanvas passes KConfig(
      *                      "kviewcanvas" )
      */
//     void addPlugins(const QString &componentName,
//                     const QString &categoryName = QString(),
//                     const QString &categoryKey = QString(),
//                     KSharedConfig::Ptr config = KSharedConfig::Ptr());

    /**
      * Add a list of KParts plugins. Convenience method for the one above.
      * If not set explicitly, @p config is set to componentData.config()
      */
//     void addPlugins(const KComponentData &instance,
//                     const QString &categoryName = QString(),
//                     const QString &categoryKey = QString(),
//                     const KSharedConfig::Ptr &config = KSharedConfig::Ptr());

    /**
      * Add a list of non-KParts plugins
      *
      * @param pluginInfoList   A list of KPluginInfo objects containing the
      *                         necessary information for the plugins you want to
      *                         add to the list
      * @param pluginLoadMethod If PluginListView will try to load the
      *                         state of the plugin when loading the
      *                         dialog from the configuration file or not.
      *                         This is useful if for some reason you
      *                         called the setPluginEnabled() for each plugin
      *                         individually before loading the dialog, and
      *                         don't want PluginListView to override them
      *                         when loading
      * @param categoryName     The translated name of the category. This is the
      *                         name that is shown in the title. If the category
      *                         did exist before because of another call to
      *                         addPlugins, then they will be shown in that
      *                         category. If @p categoryName is a new one, then
      *                         a new category will be shown on the plugin window,
      *                         and the list of plugins added to it
      * @param categoryKey      When you have different categories of KParts
      *                         plugins you distinguish between the plugins using
      *                         the Category key in the .desktop file. Use this
      *                         parameter to select only those KParts plugins
      *                         with the Category key == @p categoryKey. If
      *                         @p categoryKey is not set the Category key is
      *                         ignored and all plugins are shown. Not match case
      * @param config           The KConfig object that holds the state of the
      *                         plugins being enabled or not. By default it will
      *                         use KGlobal::config(). It is recommended to
      *                         always pass a KConfig object if you use
      *                         KSettings::PluginPage since you never know from
      *                         where the page will be called (think global
      *                         config app). For example KViewCanvas passes
      *                         KConfig("kviewcanvas")
      *
      * @note   All plugins that were set a config group using setConfig() method
      *         will load and save their information from there. For those that
      *         weren't any config object, @p config will be used
      */
//     void addPlugins(const QList<KPluginInfo> &pluginInfoList,
//                     PluginLoadMethod pluginLoadMethod = ReadConfigFile,
//                     const QString &categoryName = QString(),
//                     const QString &categoryKey = QString(),
//                     const KSharedConfig::Ptr &config = KSharedConfig::Ptr());

    /**
      * Load the state of the plugins (selected or not) from the KPluginInfo
      * objects
      */
//     void load();

    /**
      * Save the configuration
      */
//     void save();

    /**
      * Change to applications defaults
      * @see isDefault()
      */
//     void defaults();

    /**
      * Returns true if the plugin selector does not have any changes to application defaults
      * @see defaults()
      * @since 4.3
      */
//     bool isDefault() const;

    /**
      * Updates plugins state (enabled or not)
      *
      * This method won't save anything on any configuration file. It will just
      * be useful if you added plugins with the method:
      *
      * \code
      * void addPlugins(const QList<KPluginInfo> &pluginInfoList,
      *                 const QString &categoryName = QString(),
      *                 const QString &categoryKey = QString(),
      *                 const KSharedConfig::Ptr &config = KSharedConfig::Ptr());
      * \endcode
      *
      * To sum up, this method will update your plugins state depending if plugins
      * are ticked or not on the PluginListView dialog, without saving anything
      * anywhere
      */
//     void updatePluginsState();

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

//    void updateDependencies(PluginEntry *pluginEntry, bool added);
    int dependantLayoutValue(int value, int width, int totalWidth) const;

public:
    class PluginModel;
    class ProxyModel;
    class PluginDelegate;
    class DependenciesWidget;
    QObject *parent;
    QLineEdit *lineEdit;
    CategorizedListView *listView;
    CategorizedListViewPainter *categoryDrawer;
    PluginModel *pluginModel;
    ProxyModel *proxyModel;
    PluginDelegate *pluginDelegate;
//     DependenciesWidget *dependenciesWidget;
    bool showIcons;
};

// class PluginEntry
// {
// public:
//     QString category;
// //     KPluginInfo pluginInfo;
//     bool checked;
//     bool manuallyAdded;
// //     KConfigGroup cfgGroup;
//     PluginListView::PluginLoadMethod pluginLoadMethod;
//     bool isCheckable;
// 
//     bool operator==(const PluginEntry &pe) const
//     {
//         return pluginInfo.entryPath() == pe.pluginInfo.entryPath();
//     }
// };

// Q_DECLARE_METATYPE(PluginEntry*)


/**
 * This widget will inform the user about changes that happened automatically
 * due to plugin dependencies.
 */
// class PluginListView::Private::DependenciesWidget
//     : public QWidget
// {
//     Q_OBJECT
// 
// public:
//     DependenciesWidget(QWidget *parent = 0);
//     ~DependenciesWidget();
// 
//     void addDependency(const QString &dependency, const QString &pluginCausant, bool added);
//     void userOverrideDependency(const QString &dependency);
// 
//     void clearDependencies();
// 
// private Q_SLOTS:
//     void showDependencyDetails();
// 
// private:
//     struct FurtherInfo
//     {
//         bool added;
//         QString pluginCausant;
//     };
// 
//     void updateDetails();
// 
//     QLabel *details;
//     QMap<QString, struct FurtherInfo> dependencyMap;
//     int addedByDependencies;
//     int removedByDependencies;
// };

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
public:
    PluginModel(PluginListView::Private *pluginSelector_d, QObject *parent = 0);
    ~PluginModel();

    //void addPlugins(const QList<KPluginInfo> &pluginList, const QString &categoryName, const QString &categoryKey, const KConfigGroup &cfgGroup, PluginLoadMethod pluginLoadMethod = ReadConfigFile, bool manuallyAdded = false);
    //QList<KService::Ptr> pluginServices(const QModelIndex &index) const;

    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	void someShit();
// public:
//     QList<PluginEntry> pluginEntryList;

private:
    PluginListView::Private *pluginSelector_d;
        PluginsManager *Manager;
	QList<PluginEntry> Plugins;
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
    void slotDefaultClicked();

private:
    QFont titleFont(const QFont &baseFont) const;

    QCheckBox *checkBox;
    QPushButton *pushButton;
//     QList<KCModuleProxy*> moduleProxyList;

    PluginListView::Private *pluginSelector_d;
};



#endif
