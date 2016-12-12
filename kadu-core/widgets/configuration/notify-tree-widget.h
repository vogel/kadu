/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
/* This classes are based on the KNotificationEventList* classes, which are the part
 * of KDE libraries (see kde.org) and distributed under the terms
 * of the GNU Library General Public License version 2 as published
 * by the Free Software Foundation
 *  Copyright (C) 2005 Olivier Goffart <ogoffart at kde.org>
 */

#pragma once

#include "notification/notify-configuration-ui-handler.h"

#include <QtCore/QPointer>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <injeqt/injeqt.h>

class InjectedFactory;
class NotifierRepository;
class Notifier;

class NotifyTreeWidgetDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit NotifyTreeWidgetDelegate(QObject *parent = nullptr);
	virtual ~NotifyTreeWidgetDelegate();

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	QPointer<IconsManager> m_iconsManager;
	QPointer<NotifierRepository> m_notifierRepository;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);

};


class NotifyTreeWidgetItem : public QTreeWidgetItem
{
	QStringList ActiveNotifiers;
	bool useCustomSettings;

public:
	NotifyTreeWidgetItem(QTreeWidget *parent , const QString &eventName, const QString &name, QStringList &notifiers);
	NotifyTreeWidgetItem(NotifyTreeWidgetItem *parent , const QString &eventName, const QString &name, QStringList &notifiers);

	void notifierChecked(Notifier *notifier, bool checked);
	void parentNotifierChecked();
	void useCustomSettingsChecked(bool checked);

	const QStringList & activeNotifiers() const { return ActiveNotifiers; }
};

class NotifyTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	NotifyTreeWidget(NotifyConfigurationUiHandler *uiHandler, QWidget *parent);

	QString currentEvent();

	void notifierChecked(Notifier *notifier, bool checked);
	void useCustomSettingsChecked(bool checked);

public slots:
	void refresh();

protected:
	virtual void resizeEvent(QResizeEvent *event);

private:
	QPointer<IconsManager> m_iconsManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<NotifierRepository> m_notifierRepository;

	NotifyConfigurationUiHandler *UiHandler;

	int StateColumnDefaultWidth;
	int IconWidth;
	int ColumnWidth;

	inline int eventColumnWidth();

	QMap<QString, NotifyTreeWidgetItem *> TreeItems;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);
	INJEQT_INIT void init();

};
