/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2008 Rafael Fernández López <ereslibre@kde.org>
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

#ifndef PLUGIN_LIST_WIDGET_DELEGATE_WIDGETS_H
#define PLUGIN_LIST_WIDGET_DELEGATE_WIDGETS_H

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QModelIndex>

class QWidget;
class QStyleOptionViewItem;
class PluginListWidgetDelegate;
class PluginListWidgetDelegateEventListener;


class PluginListWidgetDelegateWidgets
{
        friend class PluginListWidgetDelegate;
	friend class PluginListWidgetDelegateEventListener;

	PluginListWidgetDelegate *delegate;

	PluginListWidgetDelegateEventListener *eventListener;

        QList<QList<QWidget*> > allocatedWidgets;
        QHash<QPersistentModelIndex, QList<QWidget*> > usedWidgets;
        QHash<QWidget*, QPersistentModelIndex> widgetInIndex;

        bool clearing;

public:
        enum UpdateWidgetsEnum
        {
                UpdateWidgets = 0,
                NotUpdateWidgets
        };

        PluginListWidgetDelegateWidgets(PluginListWidgetDelegate *delegate);

        ~PluginListWidgetDelegateWidgets();

        /**
          * @brief Returns the widget associated to @p index and @p widget
          * @param index The index to search into.
          * @param option a QStyleOptionViewItem.
          * @return A QList of the pointers to the widgets found.
          * @internal
          */
        QList<QWidget*> findWidgets(const QPersistentModelIndex &index, const QStyleOptionViewItem &option,
                                    UpdateWidgetsEnum updateWidgets = UpdateWidgets);

        QList<QWidget*> invalidIndexesWidgets() const;

        void fullClear();
};

class PluginListWidgetDelegateEventListener
                        : public QObject
{
	friend class PluginListWidgetDelegateWidgets;

        PluginListWidgetDelegateWidgets *Pool;

public:
        PluginListWidgetDelegateEventListener(PluginListWidgetDelegateWidgets *pool, QObject *parent = 0);

        virtual bool eventFilter(QObject *watched, QEvent *event);
};

#endif
