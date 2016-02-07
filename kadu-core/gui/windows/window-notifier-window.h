/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "os/generic/desktop-aware-object.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class IconsManager;
class Notification;
class NotificationCallbackRepository;

class WindowNotifierWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

public:
	explicit WindowNotifierWindow(const Notification &notification, QWidget *parent = nullptr);
	virtual ~WindowNotifierWindow();

private:
	QPointer<IconsManager> m_iconsManager;
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;

	const Notification &m_notification;

	void createGui();
	void addButton(QWidget *parent, const QString &title, const QString &name);

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_INIT void init();

	void buttonClicked();

};
