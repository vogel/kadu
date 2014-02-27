/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
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
/* This classes are based on the KNotifyEventList* classes, which are the part
 * of KDE libraries (see kde.org) and distributed under the terms
 * of the GNU Library General Public License version 2 as published
 * by the Free Software Foundation
 *  Copyright (C) 2005 Olivier Goffart <ogoffart at kde.org>
 */

#include <QtGui/QApplication>
#include <QtGui/QFontMetrics>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>

#include "notify/notification-manager.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/notify-event.h"

#include "notify-tree-widget.h"

#define OFFSET 30

NotifyTreeWidgetDelegate::NotifyTreeWidgetDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NotifyTreeWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() != 1)
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
			notifier->icon().icon().paint(painter, rect.left() + position + 4, rect.top() + (rect.height() - iconHeight) / 2, iconWidth, iconHeight);
		position += iconWidth + 4;
	}

}

NotifyTreeWidget::NotifyTreeWidget(NotifyConfigurationUiHandler *uiHandler, QWidget *parent)
	: QTreeWidget(parent), UiHandler(uiHandler)
{
	QStringList headerLabels;
	headerLabels << tr("Event") << tr("Notification");
	setHeaderLabels(headerLabels);

	setItemDelegate(new NotifyTreeWidgetDelegate(this));
	setAlternatingRowColors(true);
	setItemsExpandable(true);
	setExpandsOnDoubleClick(true);

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(refresh()));

	//Extract icon size as the font height (as h=w on icons)
	QStyleOptionViewItem iconOption;
	iconOption.initFrom(this);
	IconWidth = iconOption.fontMetrics.height() - 2 ; //1px margin top & bottom
	StateColumnDefaultWidth = header()->sectionSizeHint(1);

	setIconSize(QSize(IconWidth, IconWidth));

	header()->setResizeMode(0, QHeaderView::Fixed);
	header()->setResizeMode(1, QHeaderView::Fixed);

	refresh();
}

void NotifyTreeWidget::refresh()
{
	QString currentName;
	if (currentItem())
		currentName = currentItem()->text(0);
	clear();
	TreeItems.clear();

	ColumnWidth = (IconWidth + 4) * NotificationManager::instance()->notifiers().count();
	header()->resizeSection(0, eventColumnWidth());

	const QMap<Notifier *, NotifierConfigurationGuiItem> &notifierGuiItems = UiHandler->notifierGui();
	const QMap<QString, NotifyEventConfigurationItem> &notifyEventItem = UiHandler->notifyEvents();

	QStringList notifiersNames;
	QString eventName;
	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		eventName = notifyEvent->name();
		foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
			if (notifierGuiItems[notifier].Events[eventName])
				notifiersNames << notifier->name();

		if (notifyEvent->category().isEmpty())
			TreeItems.insert(eventName, new NotifyTreeWidgetItem(this, eventName,
						notifyEvent->description(), notifiersNames));
		else
		{
			TreeItems[eventName] = new NotifyTreeWidgetItem(TreeItems[notifyEvent->category()], eventName,
						notifyEvent->description(), notifiersNames);
			TreeItems[eventName]->useCustomSettingsChecked(notifyEventItem[eventName].useCustomSettings);
		}
		notifiersNames.clear();
	}

	if (!currentName.isNull())
	{
		QList<QTreeWidgetItem *> items = findItems(currentName, Qt::MatchExactly, 0);
		if (!items.isEmpty())
			setCurrentItem(items.at(0));
	}

	expandAll();
}

QString NotifyTreeWidget::currentEvent()
{
	return currentItem()->data(0, Qt::UserRole).toString();
}

void NotifyTreeWidget::notifierChecked(Notifier *notifier, bool checked)
{
	NotifyTreeWidgetItem *item = dynamic_cast<NotifyTreeWidgetItem *>(currentItem());
	if (item)
		item->notifierChecked(notifier, checked);
}

void NotifyTreeWidget::useCustomSettingsChecked(bool checked)
{
   	NotifyTreeWidgetItem *item = dynamic_cast<NotifyTreeWidgetItem *>(currentItem());
	if (item)
		item->useCustomSettingsChecked(checked);
}

int NotifyTreeWidget::eventColumnWidth()
{
	return ColumnWidth > StateColumnDefaultWidth ? width() - OFFSET - ColumnWidth : width() - OFFSET - StateColumnDefaultWidth;
}

void NotifyTreeWidget::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event)

	header()->resizeSection(0, eventColumnWidth());
}

NotifyTreeWidgetItem::NotifyTreeWidgetItem(QTreeWidget *parent, const QString &eventName, const char *name, QStringList &notifiers)
	: QTreeWidgetItem(parent), ActiveNotifiers(notifiers), useCustomSettings(true)
{
	setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

	setData(1, Qt::UserRole, QVariant(ActiveNotifiers));
	setData(0, Qt::UserRole, QVariant(eventName));
	setText(0, QCoreApplication::translate("@default", name));
}

NotifyTreeWidgetItem::NotifyTreeWidgetItem(NotifyTreeWidgetItem *parent, const QString &eventName, const char *name, QStringList &notifiers)
	: QTreeWidgetItem(parent), ActiveNotifiers(notifiers), useCustomSettings(true)
{
	setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

	setData(1, Qt::UserRole, QVariant(ActiveNotifiers));
	setData(0, Qt::UserRole, QVariant(eventName));
	setText(0, QCoreApplication::translate("@default", name));
}

void NotifyTreeWidgetItem::notifierChecked(Notifier *notifier, bool checked)
{
	if (checked)
		ActiveNotifiers << notifier->name();
	else
		ActiveNotifiers.removeAll(notifier->name());

	setData(1, Qt::UserRole, QVariant(ActiveNotifiers));

	for (int i = 0; i < childCount(); ++i)
		static_cast<NotifyTreeWidgetItem *>(child(i))->parentNotifierChecked();
}

void NotifyTreeWidgetItem::useCustomSettingsChecked(bool checked)
{
	if (!parent() || useCustomSettings == checked)
		return;

	useCustomSettings = checked;
	if (useCustomSettings)
		setData(1, Qt::UserRole, QVariant(ActiveNotifiers));
	else
		setData(1, Qt::UserRole, QVariant(dynamic_cast<NotifyTreeWidgetItem *>(parent())->activeNotifiers()));
    }

 void NotifyTreeWidgetItem::parentNotifierChecked()
 {
	if (useCustomSettings)
		return;

	setData(1, Qt::UserRole, QVariant(dynamic_cast<NotifyTreeWidgetItem *>(parent())->activeNotifiers()));
 }

#include "moc_notify-tree-widget.cpp"
