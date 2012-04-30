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

#ifndef PLUGIN_LIST_WIDGET_DELEGATE_H
#define PLUGIN_LIST_WIDGET_DELEGATE_H

#include "gui/widgets/plugin-list-view-delegate.h"

class QCheckBox;
class QPushButton;

class PluginListWidget;


class PluginListWidgetItemDelegate : public PluginListWidgetDelegate
{
        Q_OBJECT

        QCheckBox *checkBox;
        QPushButton *pushButton;
        PluginListWidget *pluginSelector_d;

        QFont titleFont(const QFont &baseFont) const;
        QFont subtitleFont(const QFont &baseFont) const;
public:
        PluginListWidgetItemDelegate(PluginListWidget *pluginSelector_d, QObject *parent = 0);
        ~PluginListWidgetItemDelegate();

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

Q_SIGNALS:
        void changed(bool hasChanged);

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
};

#endif
