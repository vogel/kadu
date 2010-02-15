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
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>

#include "../docking/docking.h"

#include "configuration/configuration-file.h"
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

	return 0;
}

extern "C" void qt4_docking_close()
{
	delete qt4_tray_icon;
	qt4_tray_icon = 0;
}


Qt4TrayIcon::Qt4TrayIcon(QWidget *parent) :
		QSystemTrayIcon(parent), Movie(0)
{
	kdebugf();

	setIcon(QIcon(docking_manager->defaultPixmap()));

	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&)), this, SLOT(setTrayPixmap(const QIcon&)));
	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(const QString &)), this, SLOT(setTrayMovie(const QString &)));

	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	show();
	setContextMenu(docking_manager->dockMenu());
	
	docking_manager->setDocked(true);

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

	disconnect(docking_manager, SIGNAL(trayMovieChanged(const QString &)), this, SLOT(setTrayMovie(const QString &)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&)), this, SLOT(setTrayPixmap(const QIcon&)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	disconnect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	docking_manager->setDocked(false);

	kdebugf2();
}

void Qt4TrayIcon::findTrayPosition(QPoint& pos)
{
	QRect rect = geometry();
	if (rect.isValid()){
		pos = QPoint(rect.x(), rect.y());
		lastPosition = pos;
	}
	else
		pos = lastPosition;
		
}

void Qt4TrayIcon::setTrayPixmap(const QIcon& pixmap)
{
	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
		Movie = 0;
	}
	setIcon(pixmap);
}

void Qt4TrayIcon::setTrayMovie(const QString &movie)
{
	if (Movie)
	{
		Movie->stop();
		Movie->deleteLater();
	}
	else
		setIcon(QIcon(""));

	Movie = new QMovie(movie);
	Movie->start();
	connect(Movie, SIGNAL(updated(const QRect &)), this, SLOT(movieUpdate()));
}

void Qt4TrayIcon::movieUpdate()
{
	setIcon(Movie->currentPixmap());
}

void Qt4TrayIcon::setTrayTooltip(const QString& tooltip)
{
	setToolTip(tooltip);
}

void Qt4TrayIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	QMouseEvent *ev = 0;
	// nie przekazujemy klikniecia ppm poniewaz QSystemTrayIcon reaguje
	// na niego i wyswietla menu kontekstowe samemu
	if (reason == QSystemTrayIcon::Trigger)
		ev = new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
	else if (reason == QSystemTrayIcon::MiddleClick)
		ev = new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::MidButton, Qt::MidButton, Qt::NoModifier);
	if (ev)
		docking_manager->trayMousePressEvent(ev);
}


Qt4TrayIcon* qt4_tray_icon = NULL;

/** @} */
