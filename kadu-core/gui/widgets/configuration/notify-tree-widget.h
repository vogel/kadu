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


class NotifyTreeWidgetItem : public QTreeWidgetItem
{
	QStringList ActiveNotifiers;
	bool useCustomSettings;

public:
	NotifyTreeWidgetItem(QTreeWidget *parent , const QString &eventName, const char *name, QStringList &notifiers);
	NotifyTreeWidgetItem(NotifyTreeWidgetItem *parent , const QString &eventName, const char *name, QStringList &notifiers);

	void notifierChecked(Notifier *notifier, bool checked);
	void parentNotifierChecked();
	void useCustomSettingsChecked(bool checked);

	QStringList activeNotifiers() { return ActiveNotifiers; }
};

class NotifyTreeWidget : public QTreeWidget
{
	NotifyConfigurationUiHandler *UiHandler;

	int StateColumnDefaultWidth;
	int IconWidth;
	int ColumnWidth;

	inline int eventColumnWidth();

	QMap<QString, NotifyTreeWidgetItem *> TreeItems;

protected:
	virtual void resizeEvent(QResizeEvent *event);

public:
	NotifyTreeWidget(NotifyConfigurationUiHandler *uiHandler, QWidget *parent);

	void refresh();

	QString currentEvent();

	void notifierChecked(Notifier *notifier, bool checked);
	void useCustomSettingsChecked(bool checked);
};

#endif //NOTIFY_TREE_WIDGET_H
