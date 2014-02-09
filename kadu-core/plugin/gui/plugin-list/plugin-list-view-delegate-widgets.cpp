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

#define POOL_USAGE 0

#include <math.h>

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QPair>
#include <QtCore/qobjectdefs.h>
#include <QtGui/QAbstractItemView>
#include <QtGui/QAbstractProxyModel>
#include <QtGui/QApplication>
#include <QtGui/QInputEvent>
#include <QtGui/QWidget>

#include "plugin/gui/plugin-list/plugin-list-view-delegate.h"

#include "plugin/gui/plugin-list/plugin-list-view-delegate-widgets.h"

Q_DECLARE_METATYPE(QModelIndex);


PluginListWidgetDelegateWidgets::PluginListWidgetDelegateWidgets(PluginListWidgetDelegate *delegate)
		: delegate(delegate), eventListener(new PluginListWidgetDelegateEventListener(this)), clearing{false}
{
}

PluginListWidgetDelegateWidgets::~PluginListWidgetDelegateWidgets()
{
        delete eventListener;
}

QList<QWidget*> PluginListWidgetDelegateWidgets::findWidgets(const QPersistentModelIndex &idx,
                const QStyleOptionViewItem &option,
                UpdateWidgetsEnum updateWidgets)
{
        QList<QWidget*> result;

        if (!idx.isValid())
        {
                return result;
        }

        QModelIndex index;

        if (const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel*>(idx.model()))
        {
                index = proxyModel->mapToSource(idx);
        }
        else
        {
                index = idx;
        }

        if (!index.isValid())
        {
                return result;
        }

        if (usedWidgets.contains(index))
        {
                result = usedWidgets[index];
        }
        else
        {
                // ### KDE5 This sets a property on the delegate because we can't add an argument to createItemWidgets
                delegate->setProperty("goya:creatingWidgetForIndex", QVariant::fromValue(index));
                result = delegate->createItemWidgets();
                delegate->setProperty("goya:creatingWidgetForIndex", QVariant());
                allocatedWidgets << result;
                usedWidgets[index] = result;
                foreach (QWidget *widget, result)
                {
                        widgetInIndex[widget] = index;
                        widget->setParent(delegate->itemView()->viewport());
                        widget->installEventFilter(eventListener);
                        widget->setVisible(true);
                }
        }

        if (updateWidgets == UpdateWidgets)
        {
                foreach (QWidget *widget, result)
                {
                        widget->setVisible(true);
                }

                delegate->updateItemWidgets(result, option, idx);

                foreach (QWidget *widget, result)
                {
                        widget->move(widget->x() + option.rect.left(), widget->y() + option.rect.top());
                }
        }

        return result;
}

QList<QWidget*> PluginListWidgetDelegateWidgets::invalidIndexesWidgets() const
{
        QList<QWidget*> result;
        foreach (QWidget *widget, widgetInIndex.keys())
        {
                const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel*>(delegate->model);
                QModelIndex index;

                if (proxyModel)
                {
                        index = proxyModel->mapFromSource(widgetInIndex[widget]);
                }
                else
                {
                        index = widgetInIndex[widget];
                }

                if (!index.isValid())
                {
                        result << widget;
                }
        }

        return result;
}

void PluginListWidgetDelegateWidgets::fullClear()
{
        clearing = true;
        qDeleteAll(widgetInIndex.keys());
        clearing = false;
        allocatedWidgets.clear();
        usedWidgets.clear();
        widgetInIndex.clear();
}

PluginListWidgetDelegateEventListener::PluginListWidgetDelegateEventListener(PluginListWidgetDelegateWidgets *pool, QObject *parent)
		: QObject(parent)
		,Pool(pool)
{
}

bool PluginListWidgetDelegateEventListener::eventFilter(QObject *watched, QEvent *event)
{
        QWidget *widget = static_cast<QWidget*>(watched);

        if (event->type() == QEvent::Destroy && !Pool->clearing)
        {
                // assume the application has kept a list of widgets and tries to delete them manually
                // they have been reparented to the view in any case, so no leaking occurs
                Pool->widgetInIndex.remove(widget);
                QWidget *viewport = Pool->delegate->itemView()->viewport();
                QApplication::sendEvent(viewport, event);
        }

        if (dynamic_cast<QInputEvent*>(event) && !Pool->delegate->blockedEventTypes(widget).contains(event->type()))
        {
                QWidget *viewport = Pool->delegate->itemView()->viewport();

                switch (event->type())
                {

                        case QEvent::MouseMove:

                        case QEvent::MouseButtonPress:

                        case QEvent::MouseButtonRelease:

                        case QEvent::MouseButtonDblClick:
                        {
                                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                                QMouseEvent evt(event->type(), viewport->mapFromGlobal(mouseEvent->globalPos()),
                                                mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                                QApplication::sendEvent(viewport, &evt);
                        }

                        break;

                        case QEvent::Wheel:
                        {
                                QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                                QWheelEvent evt(viewport->mapFromGlobal(wheelEvent->globalPos()),
                                                wheelEvent->delta(), wheelEvent->buttons(), wheelEvent->modifiers(),
                                                wheelEvent->orientation());
                                QApplication::sendEvent(viewport, &evt);
                        }

                        break;

                        case QEvent::TabletMove:

                        case QEvent::TabletPress:

                        case QEvent::TabletRelease:

                        case QEvent::TabletEnterProximity:

                        case QEvent::TabletLeaveProximity:
                        {
                                QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
                                QTabletEvent evt(event->type(),
#if QT_VERSION >= 0x050000
                                                 QPointF(viewport->mapFromGlobal(tabletEvent->globalPos())),
                                                 tabletEvent->globalPosF(),
#else
                                                 viewport->mapFromGlobal(tabletEvent->globalPos()), tabletEvent->globalPos(),
                                                 tabletEvent->hiResGlobalPos(),
#endif
                                                 tabletEvent->device(),
                                                 tabletEvent->pointerType(), tabletEvent->pressure(), tabletEvent->xTilt(),
                                                 tabletEvent->yTilt(), tabletEvent->tangentialPressure(), tabletEvent->rotation(),
                                                 tabletEvent->z(), tabletEvent->modifiers(), tabletEvent->uniqueId());
                                QApplication::sendEvent(viewport, &evt);
                        }

                        break;

                        default:
                                QApplication::sendEvent(viewport, event);
                                break;
                }
        }

        return QObject::eventFilter(watched, event);
}
