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

#include "plugin-list-view.h"
// #include "kpluginselector_p.h"

#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"

#include <QtCore/QDebug>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "model/categorized-sort-filter-proxy-model.h"

#include "icons/kadu-icon.h"

#define MARGIN 5

PluginListView::Private::Private(PluginListView *parent)
    : QObject(parent)
    , parent(parent)
    , listView(0)
    , showIcons(false)
{
}

PluginListView::Private::~Private()
{
    delete categoryDrawer;
}

int PluginListView::Private::dependantLayoutValue(int value, int width, int totalWidth) const
{
    if (listView->layoutDirection() == Qt::LeftToRight) {
        return value;
    }

    return totalWidth - width - value;
}

PluginListView::PluginListView(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout(layout);

    d->lineEdit = new QLineEdit(this);
    d->listView = new CategorizedListView(this);
    d->listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    d->listView->setAlternatingRowColors(true);
    d->categoryDrawer = new CategorizedListViewPainter(d->listView);
    d->listView->setCategoryDrawer(d->categoryDrawer);

    d->pluginModel = new Private::PluginModel(d, this);
    d->proxyModel = new Private::ProxyModel(d, this);
    d->proxyModel->setCategorizedModel(true);
    d->proxyModel->setSourceModel(d->pluginModel);
    d->pluginModel->someShit();
    d->listView->setModel(d->proxyModel);
    d->listView->setAlternatingRowColors(true);

    Private::PluginDelegate *pluginDelegate = new Private::PluginDelegate(d, this);
    d->listView->setItemDelegate(pluginDelegate);

    d->listView->setMouseTracking(true);
    d->listView->viewport()->setAttribute(Qt::WA_Hover);

    connect(d->lineEdit, SIGNAL(textChanged(QString)), d->proxyModel, SLOT(invalidate()));
    connect(pluginDelegate, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
    connect(pluginDelegate, SIGNAL(configCommitted(QByteArray)), this, SIGNAL(configCommitted(QByteArray)));

    layout->addWidget(d->lineEdit);
    layout->addWidget(d->listView);
}

PluginListView::~PluginListView()
{
    delete d->listView->itemDelegate();
    delete d->listView; // depends on some other things in d, make sure this dies first.
    delete d;
}

void PluginListView::Private::PluginModel::someShit()
{
  QList<PluginEntry> listToAdd;


	    foreach (Plugin *p, PluginsManager::instance()->plugins())
	    {
		PluginEntry pe;
		pe.category = p->info() ? "yess" : "crap";
		pe.name = p->name();
		pe.description = p->info() ? p->info()->description() : "brak opisu";
		pe.checked = p->isActive();
		pe.isCheckable = true;
		listToAdd.append(pe);
	    }
	    beginInsertRows(QModelIndex(), 0, listToAdd.size()-1);
	      Plugins << listToAdd;
	    endInsertRows();

	        pluginSelector_d->proxyModel->sort(0);
}

PluginListView::Private::PluginModel::PluginModel(PluginListView::Private *pluginSelector_d, QObject *parent)
    : QAbstractListModel(parent)
    , pluginSelector_d(pluginSelector_d)
    , Manager(PluginsManager::instance())
{
// 	for (int i = 0; i < Manager->plugins().size(); i++)
// 	{
// 	    beginInsertRows(QModelIndex(), 0, Manager->plugins().size()-1);
// 	    endInsertRows();
// 
// 	        pluginSelector_d->proxyModel->sort(0);
// 	}
//   	connect(Manager, SIGNAL(pluginAdded(const Plugin *)),
// 			this, SLOT(pluginAdded(const Plugin *)), Qt::DirectConnection);
// 	connect(Manager, SIGNAL(pluginRemoved(const Plugin *)),
// 			this, SLOT(pluginRemoved(const Plugin *)), Qt::DirectConnection);
}
PluginListView::Private::PluginModel::~PluginModel()
{
//   	disconnect(Manager, SIGNAL(pluginAdded(const Plugin *)),
// 			this, SLOT(pluginAdded(const Plugin *)));
// 	disconnect(Manager, SIGNAL(pluginRemoved(const Plugin *)),
// 			this, SLOT(pluginRemoved(const Plugin *)));
}

// void PluginsModel::pluginAdded(const Plugin *plugin)
// {
// 	Q_UNUSED(plugin)
// 
// 	beginInsertRows(QModelIndex(), 0, 0);
// 
// 	endInsertRows();
// }
// 
// void PluginsModel::pluginRemoved(const Plugin *plugin)
// {
// 	Plugin *p = Manager->plugins().value(plugin->name());
// 	int index = Manager->plugins().values().indexOf(p);
// 	beginRemoveRows(QModelIndex(), index, index);
// 
// 	endRemoveRows();
// }


QModelIndex PluginListView::Private::PluginModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return createIndex(row, column, (row < Plugins.count()) ? (void*) &Plugins.at(row) : 0);
}

QVariant PluginListView::Private::PluginModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !index.internalPointer()) {
        return QVariant();
    }

    	if (index.row() < 0 || index.row() >= Plugins.count())
		return QVariant();

    PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());


	    switch (role) {
        case Qt::DisplayRole:
	  return pluginEntry->name;
        case PluginEntryRole:
	  return QVariant::fromValue(pluginEntry);
        case NameRole:
	              return pluginEntry->name;
	case CommentRole:
		return pluginEntry->description;
        case ServicesCountRole:
        case IsCheckableRole:
            return true;
//         case Qt::DecorationRole:
//             return pluginEntry->pluginInfo.icon();
        case Qt::CheckStateRole:
            return pluginEntry->checked;
	case CategorizedSortFilterProxyModel::CategoryDisplayRole: // fall through
        case CategorizedSortFilterProxyModel::CategorySortRole:
            return pluginEntry->category;
        default:
            return QVariant();
/*
    PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
            return pluginEntry->pluginInfo.name();
        case PluginEntryRole:
            return QVariant::fromValue(pluginEntry);
        case ServicesCountRole:
            return pluginEntry->pluginInfo.kcmServices().count();
        case NameRole:
            return pluginEntry->pluginInfo.name();
        case CommentRole:
            return pluginEntry->pluginInfo.comment();
        case AuthorRole:
            return pluginEntry->pluginInfo.author();
        case EmailRole:
            return pluginEntry->pluginInfo.email();
        case WebsiteRole:
            return pluginEntry->pluginInfo.website();
        case VersionRole:
            return pluginEntry->pluginInfo.version();
        case LicenseRole:
            return pluginEntry->pluginInfo.license();
        case DependenciesRole:
            return pluginEntry->pluginInfo.dependencies();
        case IsCheckableRole:
            return pluginEntry->isCheckable;
        case Qt::DecorationRole:
            return pluginEntry->pluginInfo.icon();
        case Qt::CheckStateRole:
            return pluginEntry->checked;
        case CategorizedSortFilterProxyModel::CategoryDisplayRole: // fall through
        case CategorizedSortFilterProxyModel::CategorySortRole:
            return pluginEntry->category;
        default:
            return QVariant();*/
    }
}

bool PluginListView::Private::PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    bool ret = false;

    if (role == Qt::CheckStateRole) {
        static_cast<PluginEntry*>(index.internalPointer())->checked = value.toBool();
        ret = true;
    }

    if (ret) {
        emit dataChanged(index, index);
    }

    return ret;
}

int PluginListView::Private::PluginModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Plugins.count();
}

PluginListView::Private::ProxyModel::ProxyModel(PluginListView::Private *pluginSelector_d, QObject *parent)
    : CategorizedSortFilterProxyModel(parent)
    , pluginSelector_d(pluginSelector_d)
{
    sort(0);
}

PluginListView::Private::ProxyModel::~ProxyModel()
{
}

bool PluginListView::Private::ProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    if (!pluginSelector_d->lineEdit->text().isEmpty()) {
        const QModelIndex index = sourceModel()->index(sourceRow, 0);
        const PluginEntry *entry = static_cast<PluginEntry*>(index.internalPointer());
        return entry->name.contains(pluginSelector_d->lineEdit->text(), Qt::CaseInsensitive) ||
               entry->description.contains(pluginSelector_d->lineEdit->text(), Qt::CaseInsensitive);
    }

    return true;
}

bool PluginListView::Private::ProxyModel::subSortLessThan(const QModelIndex &left, const QModelIndex &right) const
{
      return static_cast<PluginEntry*>(left.internalPointer())->name.compare(static_cast<PluginEntry*>(right.internalPointer())->name, Qt::CaseInsensitive) < 0;
}

PluginListView::Private::PluginDelegate::PluginDelegate(PluginListView::Private *pluginSelector_d, QObject *parent)
    : PluginListViewDelegate(pluginSelector_d->listView, parent)
    , checkBox(new QCheckBox)
    , pushButton(new QPushButton)
    , pluginSelector_d(pluginSelector_d)
{
  //  pushButton->setIcon(KIcon("configure")); // only for getting size matters
}

PluginListView::Private::PluginDelegate::~PluginDelegate()
{
    delete checkBox;
    delete pushButton;
}

void PluginListView::Private::PluginDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }

    int xOffset = checkBox->sizeHint().width();
    bool disabled = !index.model()->data(index, IsCheckableRole).toBool();

    painter->save();

    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0);

    int iconSize = option.rect.height() - MARGIN * 2;
    if (pluginSelector_d->showIcons) {
//         QPixmap pixmap = KIconLoader::global()->loadIcon(index.model()->data(index, Qt::DecorationRole).toString(),
//                                                          KIconLoader::Desktop, iconSize, disabled ? KIconLoader::DisabledState : KIconLoader::DefaultState);

//         painter->drawPixmap(QRect(pluginSelector_d->dependantLayoutValue(MARGIN + option.rect.left() + xOffset, iconSize, option.rect.width()), MARGIN + option.rect.top(), iconSize, iconSize), pixmap, QRect(0, 0, iconSize, iconSize));
    } else {
        iconSize = -MARGIN;
    }

    QRect contentsRect(pluginSelector_d->dependantLayoutValue(MARGIN * 2 + iconSize + option.rect.left() + xOffset, option.rect.width() - MARGIN * 3 - iconSize - xOffset, option.rect.width()), MARGIN + option.rect.top(), option.rect.width() - MARGIN * 3 - iconSize - xOffset, option.rect.height() - MARGIN * 2);

    int lessHorizontalSpace = MARGIN * 2 + pushButton->sizeHint().width();
    if (index.model()->data(index, ServicesCountRole).toBool()) {
        lessHorizontalSpace += MARGIN + pushButton->sizeHint().width();
    }

    contentsRect.setWidth(contentsRect.width() - lessHorizontalSpace);

    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.highlightedText().color());
    }

    if (pluginSelector_d->listView->layoutDirection() == Qt::RightToLeft) {
        contentsRect.translate(lessHorizontalSpace, 0);
    }

    painter->save();
    if (disabled) {
        QPalette pal(option.palette);
        pal.setCurrentColorGroup(QPalette::Disabled);
        painter->setPen(pal.text().color());
    }

    painter->save();
    QFont font = titleFont(option.font);
    QFontMetrics fmTitle(font);
    painter->setFont(font);
    qDebug() << "plugin:" << index.model()->data(index, Qt::DisplayRole).toString();
    painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignTop, fmTitle.elidedText(index.model()->data(index, Qt::DisplayRole).toString(), Qt::ElideRight, contentsRect.width()));
    painter->restore();

    painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignBottom, option.fontMetrics.elidedText(index.model()->data(index, CommentRole).toString(), Qt::ElideRight, contentsRect.width()));

    painter->restore();
    painter->restore();
}

QSize PluginListView::Private::PluginDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int i = 5;
    int j = 1;
    if (index.model()->data(index, ServicesCountRole).toBool()) {
        i = 6;
        j = 2;
    }

    if (!pluginSelector_d->showIcons) {
        i--;
    }

    QFont font = titleFont(option.font);
    QFontMetrics fmTitle(font);

    return QSize(qMax(fmTitle.width(index.model()->data(index, Qt::DisplayRole).toString()),
                      option.fontMetrics.width(index.model()->data(index, CommentRole).toString())) +
                      /*(pluginSelector_d->showIcons ? KIconLoader::SizeMedium : 0)*/32 + MARGIN * i + pushButton->sizeHint().width() * j,
                 qMax(/*KIconLoader::SizeMedium*/32 + MARGIN * 2, fmTitle.height() + option.fontMetrics.height() + MARGIN * 2));
}

QList<QWidget*> PluginListView::Private::PluginDelegate::createItemWidgets() const
{
    QList<QWidget*> widgetList;

    QCheckBox *enabledCheckBox = new QCheckBox;
    connect(enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotStateChanged(bool)));
    connect(enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(emitChanged()));

    QPushButton *aboutPushButton = new QPushButton("about");
    aboutPushButton->setIcon(KaduIcon("help-contents").icon());
    connect(aboutPushButton, SIGNAL(clicked(bool)), this, SLOT(slotAboutClicked()));

    QPushButton *configurePushButton = new QPushButton("config");
    configurePushButton->setIcon(KaduIcon("preferences-other").icon());
    connect(configurePushButton, SIGNAL(clicked(bool)), this, SLOT(slotConfigureClicked()));

    setBlockedEventTypes(enabledCheckBox, QList<QEvent::Type>() << QEvent::MouseButtonPress
                            << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                            << QEvent::KeyPress << QEvent::KeyRelease);

    setBlockedEventTypes(aboutPushButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                            << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                            << QEvent::KeyPress << QEvent::KeyRelease);
                            
    setBlockedEventTypes(configurePushButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                            << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                            << QEvent::KeyPress << QEvent::KeyRelease);

    widgetList << enabledCheckBox << configurePushButton << aboutPushButton;

    return widgetList;
}

void PluginListView::Private::PluginDelegate::updateItemWidgets(const QList<QWidget*> widgets,
                                                                 const QStyleOptionViewItem &option,
                                                                 const QPersistentModelIndex &index) const
{
    QCheckBox *checkBox = static_cast<QCheckBox*>(widgets[0]);
    checkBox->resize(checkBox->sizeHint());
    checkBox->move(pluginSelector_d->dependantLayoutValue(MARGIN, checkBox->sizeHint().width(), option.rect.width()), option.rect.height() / 2 - checkBox->sizeHint().height() / 2);

    QPushButton *aboutPushButton = static_cast<QPushButton*>(widgets[2]);
    QSize aboutPushButtonSizeHint = aboutPushButton->sizeHint();
    aboutPushButton->resize(aboutPushButtonSizeHint);
    aboutPushButton->move(pluginSelector_d->dependantLayoutValue(option.rect.width() - MARGIN - aboutPushButtonSizeHint.width(), aboutPushButtonSizeHint.width(), option.rect.width()), option.rect.height() / 2 - aboutPushButtonSizeHint.height() / 2);

    QPushButton *configurePushButton = static_cast<QPushButton*>(widgets[1]);
    QSize configurePushButtonSizeHint = configurePushButton->sizeHint();
    configurePushButton->resize(configurePushButtonSizeHint);
    configurePushButton->move(pluginSelector_d->dependantLayoutValue(option.rect.width() - MARGIN * 2 - configurePushButtonSizeHint.width() - aboutPushButtonSizeHint.width(), configurePushButtonSizeHint.width(), option.rect.width()), option.rect.height() / 2 - configurePushButtonSizeHint.height() / 2);

    if (!index.isValid() || !index.internalPointer()) {
        checkBox->setVisible(false);
        aboutPushButton->setVisible(false);
        configurePushButton->setVisible(false);
    } else {
        checkBox->setChecked(index.model()->data(index, Qt::CheckStateRole).toBool());
        checkBox->setEnabled(index.model()->data(index, IsCheckableRole).toBool());
        configurePushButton->setVisible(index.model()->data(index, ServicesCountRole).toBool());
        configurePushButton->setEnabled(index.model()->data(index, Qt::CheckStateRole).toBool());
    }
}

void PluginListView::Private::PluginDelegate::slotStateChanged(bool state)
{
    if (!focusedIndex().isValid())
        return;

    const QModelIndex index = focusedIndex();

//     pluginSelector_d->dependenciesWidget->clearDependencies();

//     PluginEntry *pluginEntry = index.model()->data(index, PluginEntryRole).value<PluginEntry*>();
//     pluginSelector_d->updateDependencies(pluginEntry, state);

    const_cast<QAbstractItemModel*>(index.model())->setData(index, state, Qt::CheckStateRole);
}

void PluginListView::Private::PluginDelegate::emitChanged()
{
    emit changed(true);
}

void PluginListView::Private::PluginDelegate::slotAboutClicked()
{
  /*  const QModelIndex index = focusedIndex();
    const QAbstractItemModel *model = index.model();

    // Try to retrieve the plugin information from the KComponentData object of the plugin.
    // If there is no valid information, go and fetch it from the service itself (the .desktop
    // file).

    PluginEntry *entry = index.model()->data(index, PluginEntryRole).value<PluginEntry*>();
    KService::Ptr entryService = entry->pluginInfo.service();
    if (entryService) {
        KPluginLoader loader(*entryService);
        KPluginFactory *factory = loader.factory();
        if (factory) {
            const KAboutData *aboutData = factory->componentData().aboutData();
            if (!aboutData->programName().isEmpty()) { // Be sure the about data is not completely empty
                KAboutApplicationDialog aboutPlugin(aboutData, itemView());
                aboutPlugin.setPlainCaption(i18nc("Used only for plugins", "About %1", aboutData->programName()));
                aboutPlugin.exec();
                return;
            }
        }
    }

    const QString name = model->data(index, NameRole).toString();
    const QString comment = model->data(index, CommentRole).toString();
    const QString author = model->data(index, AuthorRole).toString();
    const QString email = model->data(index, EmailRole).toString();
    const QString website = model->data(index, WebsiteRole).toString();
    const QString version = model->data(index, VersionRole).toString();
    const QString license = model->data(index, LicenseRole).toString();

    KAboutData aboutData(name.toUtf8(), name.toUtf8(), ki18n(name.toUtf8()), version.toUtf8(), ki18n(comment.toUtf8()), KAboutLicense::byKeyword(license).key(), ki18n(QByteArray()), ki18n(QByteArray()), website.toLatin1());
    aboutData.setProgramIconName(index.model()->data(index, Qt::DecorationRole).toString());
    const QStringList authors = author.split(',');
    const QStringList emails = email.split(',');
    if (authors.count() == emails.count()) {
	int i = 0;
        foreach (const QString &author, authors) {
            if (!author.isEmpty()) {
                aboutData.addAuthor(ki18n(author.toUtf8()), ki18n(QByteArray()), emails[i].toUtf8(), 0);
            }
            i++;
        }
    }
    KAboutApplicationDialog aboutPlugin(&aboutData, itemView());
    aboutPlugin.setPlainCaption(i18nc("Used only for plugins", "About %1", aboutData.programName()));
    aboutPlugin.exec();*/
}

void PluginListView::Private::PluginDelegate::slotConfigureClicked()
{
 /*   const QModelIndex index = focusedIndex();
    const QAbstractItemModel *model = index.model();

    PluginEntry *pluginEntry = model->data(index, PluginEntryRole).value<PluginEntry*>();
    KPluginInfo pluginInfo = pluginEntry->pluginInfo;

    KDialog configDialog(itemView());
    configDialog.setWindowTitle(model->data(index, NameRole).toString());
    // The number of KCModuleProxies in use determines whether to use a tabwidget
    KTabWidget *newTabWidget = 0;
    // Widget to use for the setting dialog's main widget,
    // either a KTabWidget or a KCModuleProxy
    QWidget * mainWidget = 0;
    // Widget to use as the KCModuleProxy's parent.
    // The first proxy is owned by the dialog itself
    QWidget *moduleProxyParentWidget = &configDialog;

    foreach (const KService::Ptr &servicePtr, pluginInfo.kcmServices()) {
        if(!servicePtr->noDisplay()) {
            KCModuleInfo moduleInfo(servicePtr);
            KCModuleProxy *currentModuleProxy = new KCModuleProxy(moduleInfo, moduleProxyParentWidget);
            if (currentModuleProxy->realModule()) {
                moduleProxyList << currentModuleProxy;
                if (mainWidget && !newTabWidget) {
                    // we already created one KCModuleProxy, so we need a tab widget.
                    // Move the first proxy into the tab widget and ensure this and subsequent
                    // proxies are in the tab widget
                    newTabWidget = new KTabWidget(&configDialog);
                    moduleProxyParentWidget = newTabWidget;
                    mainWidget->setParent( newTabWidget );
                    KCModuleProxy *moduleProxy = qobject_cast<KCModuleProxy*>(mainWidget);
                    if (moduleProxy) {
                        newTabWidget->addTab(mainWidget, moduleProxy->moduleInfo().moduleName());
                        mainWidget = newTabWidget;
                    } else {
                        delete newTabWidget;
                        newTabWidget = 0;
                        moduleProxyParentWidget = &configDialog;
                        mainWidget->setParent(0);
                    }
                }

                if (newTabWidget) {
                    newTabWidget->addTab(currentModuleProxy, servicePtr->name());
                } else {
                    mainWidget = currentModuleProxy;
                }
            } else {
                delete currentModuleProxy;
            }
        }
    }

    // it could happen that we had services to show, but none of them were real modules.
    if (moduleProxyList.count()) {
        configDialog.setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Default);

        QWidget *showWidget = new QWidget(&configDialog);
        QVBoxLayout *layout = new QVBoxLayout;
        showWidget->setLayout(layout);
        layout->addWidget(mainWidget);
        layout->insertSpacing(-1, KDialog::marginHint());
        configDialog.setMainWidget(showWidget);

        connect(&configDialog, SIGNAL(defaultClicked()), this, SLOT(slotDefaultClicked()));

        if (configDialog.exec() == QDialog::Accepted) {
            foreach (KCModuleProxy *moduleProxy, moduleProxyList) {
                QStringList parentComponents = moduleProxy->moduleInfo().service()->property("X-KDE-ParentComponents").toStringList();
                moduleProxy->save();
                foreach (const QString &parentComponent, parentComponents) {
                    emit configCommitted(parentComponent.toLatin1());
                }
            }
        } else {
            foreach (KCModuleProxy *moduleProxy, moduleProxyList) {
                moduleProxy->load();
            }
        }

        qDeleteAll(moduleProxyList);
        moduleProxyList.clear();
    }*/
}

void PluginListView::Private::PluginDelegate::slotDefaultClicked()
{
//     foreach (KCModuleProxy *moduleProxy, moduleProxyList) {
//         moduleProxy->defaults();
//     }
}

QFont PluginListView::Private::PluginDelegate::titleFont(const QFont &baseFont) const
{
    QFont retFont(baseFont);
    retFont.setBold(true);

    return retFont;
}

// #include "kpluginselector_p.moc"
// #include "kpluginselector.moc"
