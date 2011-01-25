/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QEvent>
#include <QtCore/QScopedPointer>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>

#include "../docking/docking.h"

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "debug.h"

#include "qt4_docking.h"

/**
 * @ingroup qt4_docking
 * @{
 */

extern "C" int qt4_docking_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	qt4_tray_icon = new Qt4TrayIcon(0);
	DockingManager::instance()->setDocker(qt4_tray_icon);

	return 0;
}

extern "C" void qt4_docking_close()
{
	if (!Core::instance()->isClosing())
		DockingManager::instance()->setDocker(0);
	delete qt4_tray_icon;
	qt4_tray_icon = 0;
}


Qt4TrayIcon::Qt4TrayIcon(QWidget *parent) :
		QSystemTrayIcon(parent), Movie(0)
{
	kdebugf();

	setIcon(QIcon(DockingManager::instance()->defaultPixmap()));

	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	show();
	setContextMenu(DockingManager::instance()->dockMenu());

	kdebugf2();
}

Qt4TrayIcon::~Qt4TrayIcon()
{
	kdebugf();

	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
		Movie = 0;
	}

	disconnect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	kdebugf2();
}

void Qt4TrayIcon::changeTrayIcon(const QIcon &icon)
{
	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
		Movie = 0;
	}
	setIcon(icon);
}

void Qt4TrayIcon::changeTrayMovie(const QString &moviePath)
{
	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
	}
	else
		setIcon(QIcon(QString()));

	Movie = new QMovie(moviePath);
	Movie->start();
	connect(Movie, SIGNAL(updated(const QRect &)), this, SLOT(movieUpdate()));
}

void Qt4TrayIcon::changeTrayTooltip(const QString &tooltip)
{
	setToolTip(tooltip);
}

QPoint Qt4TrayIcon::trayPosition()
{
	QRect rect = geometry();
	if (rect.isValid())
		lastPosition = QPoint(rect.x(), rect.y());

	return lastPosition;
}

void Qt4TrayIcon::movieUpdate()
{
	setIcon(Movie->currentPixmap());
}

void Qt4TrayIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	QScopedPointer<QMouseEvent> event;

	/* NOTE: We don't pass right button click 'cause QSystemTrayIcon
	 * takes care of it and displays context menu for us.
	 */
	if (reason == QSystemTrayIcon::Trigger)
		event.reset(new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
	else if (reason == QSystemTrayIcon::MiddleClick)
		event.reset(new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::MidButton, Qt::MidButton, Qt::NoModifier));

	if (!event.isNull())
		DockingManager::instance()->trayMousePressEvent(event.data());
}


Qt4TrayIcon *qt4_tray_icon = 0;

/** @} */
