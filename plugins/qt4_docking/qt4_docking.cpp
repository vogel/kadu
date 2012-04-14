/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>

#include "plugins/docking/docking.h"

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "debug.h"
#include "exports.h"

#include "qt4_docking.h"

/**
 * @ingroup qt4_docking
 * @{
 */

Qt4TrayIcon * Qt4TrayIcon::Instance = 0;

Qt4TrayIcon * Qt4TrayIcon::createInstance()
{
	Instance = new Qt4TrayIcon();

	return Instance;
}

Qt4TrayIcon * Qt4TrayIcon::instance()
{
	return Instance;
}

void Qt4TrayIcon::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Qt4TrayIcon::Qt4TrayIcon(QWidget *parent) :
		QSystemTrayIcon(parent), Movie(0)
{
	kdebugf();

	setIcon(DockingManager::instance()->defaultIcon().icon());

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

void Qt4TrayIcon::changeTrayIcon(const KaduIcon &icon)
{
	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
		Movie = 0;
	}
	setIcon(icon.icon());
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
#ifdef Q_WS_WIN
	// checked on XP and 7
	const int maxTooltipLength = 127;
	const QString &truncatedTooltip = tooltip.length() > maxTooltipLength
			? tooltip.left(maxTooltipLength - 3) + QLatin1String("...")
			: tooltip;
	setToolTip(truncatedTooltip);
#else
	setToolTip(tooltip);
#endif
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
	/* NOTE: We don't pass right button click 'cause QSystemTrayIcon
	 * takes care of it and displays context menu for us.
	 */
	if (reason == QSystemTrayIcon::Trigger)
	{
		QMouseEvent event(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
		DockingManager::instance()->trayMousePressEvent(&event);
	}
	else if (reason == QSystemTrayIcon::MiddleClick)
	{
		QMouseEvent event(QEvent::MouseButtonPress, QPoint(0,0), Qt::MidButton, Qt::MidButton, Qt::NoModifier);
		DockingManager::instance()->trayMousePressEvent(&event);
	}
}

/** @} */
