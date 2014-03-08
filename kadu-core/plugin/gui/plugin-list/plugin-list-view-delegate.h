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

#ifndef PLUGIN_LIST_VIEW_DELEGATE_H
#define PLUGIN_LIST_VIEW_DELEGATE_H

#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QtGui/QAbstractItemDelegate>


#include <QtGui/QItemSelectionModel>

class PluginListWidgetDelegate;
class PluginListWidgetDelegateWidgets;

// class PluginListWidgetDelegatePrivate
//                         : public QObject
// {
//         Q_OBJECT
//
// public:
//         explicit PluginListWidgetDelegatePrivate(PluginListWidgetDelegate *q, QObject *parent = 0);
//         ~PluginListWidgetDelegatePrivate();

//         void _k_slotRowsInserted(const QModelIndex &parent, int start, int end);
//         void _k_slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
//         void _k_slotRowsRemoved(const QModelIndex &parent, int start, int end);
//         void _k_slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
//         void _k_slotLayoutChanged();
//         void _k_slotModelReset();
//
//         void updateRowRange(const QModelIndex &parent, int start, int end, bool isRemoving);

// public Q_SLOTS:
//         void initializeModel(const QModelIndex &parent = QModelIndex());

// protected:
//         virtual bool eventFilter(QObject *watched, QEvent *event);
/*
public:
        QAbstractItemView *itemView;
        PluginListWidgetDelegateWidgets *widgetPool;
        QAbstractItemModel *model;
        bool viewDestroyed;

        PluginListWidgetDelegate *q;
};*/

class QObject;
class QPainter;
class QStyleOption;
class QStyleOptionViewItem;
class QAbstractItemView;
class PluginListWidgetDelegateWidgets;

/**
 * This class allows to create item delegates embedding simple widgets to interact
 * with items. For instance you can add push buttons, line edits, etc. to your delegate
 * and use them to modify the state of your model.
 */

class PluginListWidgetDelegate : public QAbstractItemDelegate
{
        Q_OBJECT

	friend class PluginListWidgetDelegateWidgets;
        friend class PluginListWidgetDelegateEventListener;

        QAbstractItemView *ItemView;
        PluginListWidgetDelegateWidgets *widgetPool;
        QAbstractItemModel *model;
        bool viewDestroyed;

        void updateRowRange(const QModelIndex &parent, int start, int end, bool isRemoving);
public:
        /**
         * Creates a new ItemDelegate to be used with a given itemview.
         *
         * @param itemView the item view the new delegate will monitor
         * @param parent the parent of this delegate
         */
        explicit PluginListWidgetDelegate(QAbstractItemView *itemView, QObject *parent = 0);

        /**
         * Destroys an ItemDelegate.
         */
        virtual ~PluginListWidgetDelegate();

        /**
         * Retrieves the item view this delegate is monitoring.
         *
         * @return the item view this delegate is monitoring
         */
        QAbstractItemView *itemView() const;

        /**
         * Retrieves the currently focused index. An invalid index if none is focused.
         *
         * @return the current focused index, or QPersistentModelIndex() if none is focused.
         */
        QPersistentModelIndex focusedIndex() const;

public slots:
	void initializeModel(const QModelIndex &parent = QModelIndex());

private slots:
  	void _k_slotRowsInserted(const QModelIndex &parent, int start, int end);
        void _k_slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
        void _k_slotRowsRemoved(const QModelIndex &parent, int start, int end);
        void _k_slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
        void _k_slotLayoutChanged();
        void _k_slotModelReset();

protected:
        /**
         * Creates the list of widgets needed for an item.
         *
         * @note No initialization of the widgets is supposed to happen here.
         *       The widgets will be initialized based on needs for a given item.
         *
         * @note If you want to connect some widget signals to any slot, you should
         *       do it here.
         *
         * @note If you want to know the index for which you are creating widgets, it is
         *       available as a QModelIndex Q_PROPERTY called "goya:creatingWidgetsForIndex".
         *       Ensure to add Q_DECLARE_METATYPE(QModelIndex) before your method definition
         *       to tell QVariant about QModelIndex.
         *
         * @return the list of newly created widgets which will be used to interact with an item.
         * @see updateItemWidgets()
         */
        virtual QList<QWidget*> createItemWidgets() const = 0;

        /**
         * Updates a list of widgets for its use inside of the delegate (painting or
         * event handling).
         *
         * @note All the positioning and sizing should be done in item coordinates.
         *
         * @warning Do not make widget connections in here, since this method will
         * be called very regularly.
         *
         * @param widgets the widgets to update
         * @param option the current set of style options for the view.
         * @param index the model index of the item currently manipulated.
         */
        virtual void updateItemWidgets(const QList<QWidget*> widgets,
                                       const QStyleOptionViewItem &option,
                                       const QPersistentModelIndex &index) const = 0;

        /**
         * Paint the widgets of the item. This method is meant to be used in the paint()
         * method of your item delegate implementation.
         *
         * @param painter the painter the widgets will be painted on.
         * @param option the current set of style options for the view.
         * @param index the model index of the item currently painted.
         *
         * @warning since 4.2 this method is not longer needed to be called. All widgets will kept
         *          updated without the need of calling paintWidgets() in your paint() event. For the
         *          widgets of a certain index to be updated your model has to emit dataChanged() on the
         *          indexes that want to be updated.
         */


        /**
         * Sets the list of event @p types that a @p widget will block.
         *
         * Blocked events are not passed to the view. This way you can prevent an item
         * from being selected when a button is clicked for instance.
         *
         * @param widget the widget which must block events
         * @param types the list of event types the widget must block
         */
        void setBlockedEventTypes(QWidget *widget, QList<QEvent::Type> types) const;

        /**
         * Retrieves the list of blocked event types for the given widget.
         *
         * @param widget the specified widget.
         *
         * @return the list of blocked event types, can be empty if no events are blocked.
         */
        QList<QEvent::Type> blockedEventTypes(QWidget *widget) const;

        virtual bool eventFilter(QObject *watched, QEvent *event);
};

#endif
