/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007-2008 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2008 Kevin Ottens <ervin@kde.org>
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

#include <QIcon>
#include <QSize>
#include <QStyle>
#include <QEvent>
#include <QHoverEvent>
#include <QFocusEvent>
#include <QCursor>
#include <QTimer>
#include <QBitmap>
#include <QLayout>
#include <QPainter>
#include <QScrollBar>
#include <QKeyEvent>
#include <QApplication>
#include <QStyleOption>
#include <QPaintEngine>
#include <QCoreApplication>
#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QTreeView>

#include "gui/widgets/plugin-list-view-delegate-widgets.h"

#include "plugin-list-view-delegate.h"


Q_DECLARE_METATYPE(QList<QEvent::Type>)

void PluginListWidgetDelegate::_k_slotRowsInserted(const QModelIndex &parent, int start, int end)
{
        Q_UNUSED(end);
        // We need to update the rows behind the inserted row as well because the widgets need to be
        // moved to their new position
        updateRowRange(parent, start, model->rowCount(parent), false);
}

void PluginListWidgetDelegate::_k_slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
        updateRowRange(parent, start, end, true);
}

void PluginListWidgetDelegate::_k_slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
        Q_UNUSED(end);
        // We need to update the rows that come behind the deleted rows because the widgets need to be
        // moved to the new position
        updateRowRange(parent, start, model->rowCount(parent), false);
}

void PluginListWidgetDelegate::_k_slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i)
        {
                for (int j = topLeft.column(); j <= bottomRight.column(); ++j)
                {
                        const QModelIndex index = model->index(i, j, topLeft.parent());
                        QStyleOptionViewItemV4 optionView;
                        optionView.initFrom(ItemView->viewport());
                        optionView.rect = ItemView->visualRect(index);
                        widgetPool->findWidgets(index, optionView);
                }
        }
}

void PluginListWidgetDelegate::_k_slotLayoutChanged()
{
        foreach (QWidget *widget, widgetPool->invalidIndexesWidgets())
        {
                widget->setVisible(false);
        }

        QTimer::singleShot(0, this, SLOT(initializeModel()));
}

void PluginListWidgetDelegate::_k_slotModelReset()
{
        widgetPool->fullClear();
        QTimer::singleShot(0, this, SLOT(initializeModel()));
}

void PluginListWidgetDelegate::updateRowRange(const QModelIndex &parent, int start, int end, bool isRemoving)
{
        int i = start;

        while (i <= end)
        {
                for (int j = 0; j < model->columnCount(parent); ++j)
                {
                        const QModelIndex index = model->index(i, j, parent);
                        QStyleOptionViewItemV4 optionView;
                        optionView.initFrom(ItemView->viewport());
                        optionView.rect = ItemView->visualRect(index);

                        QList<QWidget*> widgetList = widgetPool->findWidgets(index, optionView, isRemoving ? PluginListWidgetDelegateWidgets::NotUpdateWidgets
                                                     : PluginListWidgetDelegateWidgets::UpdateWidgets);

                        if (isRemoving)
                        {
                                widgetPool->allocatedWidgets.removeAll(widgetList);
                                foreach (QWidget *widget, widgetList)
                                {
                                        const QModelIndex idx = widgetPool->widgetInIndex[widget];
                                        widgetPool->usedWidgets.remove(idx);
                                        widgetPool->widgetInIndex.remove(widget);
                                        delete widget;
                                }
                        }
                }

                i++;
        }
}

void PluginListWidgetDelegate::initializeModel(const QModelIndex &parent)
{
        if (!model)
        {
                return;
        }

        for (int i = 0; i < model->rowCount(parent); ++i)
        {
                for (int j = 0; j < model->columnCount(parent); ++j)
                {
                        const QModelIndex index = model->index(i, j, parent);

                        if (index.isValid())
                        {
                                QStyleOptionViewItemV4 optionView;
                                optionView.initFrom(ItemView->viewport());
                                optionView.rect = ItemView->visualRect(index);
                                widgetPool->findWidgets(index, optionView);
                        }
                }

                // Check if we need to go recursively through the children of parent (if any) to initialize
                // all possible indexes that are shown.
                const QModelIndex index = model->index(i, 0, parent);

                if (index.isValid() && model->hasChildren(index))
                {
                        initializeModel(index);
                }
        }
}

PluginListWidgetDelegate::PluginListWidgetDelegate(QAbstractItemView *v, QObject *parent)
                : QAbstractItemDelegate(parent)
                , ItemView(0)
                , widgetPool(new PluginListWidgetDelegateWidgets(this))
                , model(0)
                , viewDestroyed(false)
{
        Q_ASSERT(v);

        v->setMouseTracking(true);
        v->viewport()->setAttribute(Qt::WA_Hover);

        ItemView = v;

        ItemView->viewport()->installEventFilter(this); // mouse events
        ItemView->installEventFilter(this);             // keyboard events

        if (qobject_cast<QTreeView*>(ItemView))
        {
                connect(ItemView,  SIGNAL(collapsed(QModelIndex)),
                        this, SLOT(initializeModel()));
                connect(ItemView,  SIGNAL(expanded(QModelIndex)),
                        this, SLOT(initializeModel()));
        }
}

PluginListWidgetDelegate::~PluginListWidgetDelegate()
{
        if (!viewDestroyed)
        {
                widgetPool->fullClear();
        }

        delete widgetPool;
}

QAbstractItemView *PluginListWidgetDelegate::itemView() const
{
        return ItemView;
}

QPersistentModelIndex PluginListWidgetDelegate::focusedIndex() const
{
        const QPersistentModelIndex idx = widgetPool->widgetInIndex.value(QApplication::focusWidget());

        if (idx.isValid())
        {
                return idx;
        }

        // Use the mouse position, if the widget refused to take keyboard focus.
        const QPoint pos = ItemView->viewport()->mapFromGlobal(QCursor::pos());

        return ItemView->indexAt(pos);
}

bool PluginListWidgetDelegate::eventFilter(QObject *watched, QEvent *event)
{
        if (event->type() == QEvent::Destroy)
        {
                // we care for the view since it deletes the widgets (parentage).
                // if the view hasn't been deleted, it might be that just the
                // delegate is removed from it, in which case we need to remove the widgets
                // manually, otherwise they still get drawn.
                if (watched == ItemView)
                {
                        viewDestroyed = true;
                }

                return false;
        }

        Q_ASSERT(ItemView);

        if (model != ItemView->model())
        {
                if (model)
                        disconnect(model, 0, this, 0);

                model = ItemView->model();

                connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(_k_slotRowsInserted(QModelIndex, int, int)));
                connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(_k_slotRowsAboutToBeRemoved(QModelIndex, int, int)));
                connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(_k_slotRowsRemoved(QModelIndex, int, int)));
                connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(_k_slotDataChanged(QModelIndex, QModelIndex)));
                connect(model, SIGNAL(layoutChanged()), this, SLOT(_k_slotLayoutChanged()));
                connect(model, SIGNAL(modelReset()), this, SLOT(_k_slotModelReset()));
                QTimer::singleShot(0, this, SLOT(initializeModel()));
        }

        switch (event->type())
        {

                case QEvent::Polish:

                case QEvent::Resize:

                        if (!qobject_cast<QAbstractItemView*>(watched))
                        {
                                QTimer::singleShot(0, this, SLOT(initializeModel()));
                        }

                        break;

                default:
                        break;
        }

        return QObject::eventFilter(watched, event);
}

void PluginListWidgetDelegate::setBlockedEventTypes(QWidget *widget, QList<QEvent::Type> types) const
{
        widget->setProperty("goya:blockedEventTypes", qVariantFromValue(types));
}

QList<QEvent::Type> PluginListWidgetDelegate::blockedEventTypes(QWidget *widget) const
{
        return widget->property("goya:blockedEventTypes").value<QList<QEvent::Type> >();
}
