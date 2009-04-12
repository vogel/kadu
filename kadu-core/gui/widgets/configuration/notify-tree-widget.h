/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* This classes are based on the KNotifyEventList* classes, which are the part
 * of KDE libraries (see kde.org) and distributed under the terms
 * of the GNU Library General Public License version 2 as published
 * by the Free Software Foundation
 *  Copyright (C) 2005 Olivier Goffart <ogoffart at kde.org>
 */

#ifndef NOTIFY_TREE_WIDGET_H
#define NOTIFY_TREE_WIDGET_H

#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

#include "notify/notify-configuration-ui-handler.h"

class Notifier;

class NotifyTreeWidgetDelegate : public QStyledItemDelegate
{
public:
	NotifyTreeWidgetDelegate(QObject *parent = 0);

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class NotifyTreeWidget : public QTreeWidget
{
	Q_OBJECT

	int StateColumnDefaultWidth;
	int IconWidth;

public:
	NotifyTreeWidget(QMap<Notifier *, NotifyConfigurationUiHandler::NotifierGuiItem> notifierGui, QWidget *parent);

	void refresh(QMap<Notifier *, NotifyConfigurationUiHandler::NotifierGuiItem> notifierGui);

	QString currentEvent();

public slots:
	void updateCurrentItem(Notifier *notifier, bool checked);
};

class NotifyTreeWidgetItem : public QTreeWidgetItem
{
	QStringList ActiveNotifiers;

public:
	NotifyTreeWidgetItem(QTreeWidget *parent , const QString &eventName, const QString &name, QStringList &notifiers);

	void update(Notifier *notifier, bool checked);
};

#endif //NOTIFY_TREE_WIDGET_H
