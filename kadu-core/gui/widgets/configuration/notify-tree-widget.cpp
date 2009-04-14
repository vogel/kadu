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

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QHeaderView>
#include <QtGui/QFontMetrics>

#include "notify/notification-manager.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/notify-event.h"

#include "notify-tree-widget.h"

NotifyTreeWidgetDelegate::NotifyTreeWidgetDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NotifyTreeWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() != 0)
		return QStyledItemDelegate::paint(painter, option, index);

	QStringList notifiers = index.data(Qt::UserRole).toStringList();

	QStyledItemDelegate::paint(painter, option, index);

	QRect rect = option.rect;

	int position = 0;

	int iconWidth = option.decorationSize.width();
	int iconHeight = option.decorationSize.height();

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (notifiers.contains(notifier->name()))
			notifier->icon().paint(painter, rect.left() + position + 4, rect.top() + (rect.height() - iconHeight) / 2, iconWidth, iconHeight);
		position += iconWidth + 4;
	}

}

NotifyTreeWidget::NotifyTreeWidget(QMap<Notifier *, NotifyConfigurationUiHandler::NotifierGuiItem> notifierGui, QWidget *parent)
{
	QStringList headerLabels;
	headerLabels << tr("State") << tr("Event");
	setHeaderLabels(headerLabels);

	setItemDelegate(new NotifyTreeWidgetDelegate(this));
	setRootIsDecorated(false);
	setAlternatingRowColors(true);

	//Extract icon size as the font height (as h=w on icons)
	QStyleOptionViewItem iconOption;
	iconOption.initFrom(this);
	IconWidth = iconOption.fontMetrics.height() - 2 ; //1px margin top & bottom
	StateColumnDefaultWidth = header()->sectionSizeHint(0);

	setIconSize(QSize(IconWidth, IconWidth));

	int columnWidth = (IconWidth + 4) * NotificationManager::instance()->notifiers().count();

	header()->setResizeMode(0, QHeaderView::Fixed);
	header()->resizeSection(0, columnWidth > StateColumnDefaultWidth ? columnWidth : StateColumnDefaultWidth);
	header()->setResizeMode(1, QHeaderView::ResizeToContents);

	refresh(notifierGui);
}

void NotifyTreeWidget::refresh(QMap<Notifier *, NotifyConfigurationUiHandler::NotifierGuiItem> notifierGui)
{
	QString currentName = QString::null;
	if (currentItem())
		currentName = currentItem()->text(1);
	clear();

	int columnWidth = (IconWidth + 4) * NotificationManager::instance()->notifiers().count();
	if (columnWidth > StateColumnDefaultWidth)
		header()->resizeSection(0, columnWidth);

	QStringList notifiersNames;

	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
			if (notifierGui[notifier].Events[notifyEvent->name()])
				notifiersNames << notifier->name();

		new NotifyTreeWidgetItem(this, notifyEvent->name(), qApp->translate("@default", notifyEvent->description()), notifiersNames);
		notifiersNames.clear();
	}

	if (!currentName.isNull())
	{
		QList<QTreeWidgetItem *> items = findItems(currentName, Qt::MatchExactly, 1);
		if (items.count())
			setCurrentItem(items[0]);
	}
}

QString NotifyTreeWidget::currentEvent()
{
	return currentItem()->data(1, Qt::UserRole).toString();
}

void NotifyTreeWidget::updateCurrentItem(Notifier *notifier, bool checked)
{
	NotifyTreeWidgetItem *item = dynamic_cast<NotifyTreeWidgetItem *>(currentItem());
	if (item)
		item->update(notifier, checked);
}


NotifyTreeWidgetItem::NotifyTreeWidgetItem(QTreeWidget *parent, const QString &eventName, const QString &name, QStringList &notifiers)
	: QTreeWidgetItem(parent), ActiveNotifiers(notifiers)
{
    	setData(0, Qt::UserRole, QVariant(ActiveNotifiers));
	setData(1, Qt::UserRole, QVariant(eventName));
	setText(1, name);
}

void NotifyTreeWidgetItem::update(Notifier *notifier, bool checked)
{
	if (checked)
		ActiveNotifiers << notifier->name();
	else
		ActiveNotifiers.removeAll(notifier->name());

	setData(0, Qt::UserRole, QVariant(ActiveNotifiers));
}
