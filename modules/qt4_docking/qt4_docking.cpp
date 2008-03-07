/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qt4_docking.h"

#include <QToolTip>

#include "../docking/docking.h"
#include "debug.h"
#include "config_file.h"
#include "kadu.h"
#include "chat_manager.h"
#include "chat_widget.h"

/**
 * @ingroup qt4_docking
 * @{
 */

extern "C" int qt4_docking_init()
{
	qt4_tray_icon = new Qt4TrayIcon(NULL, "kadu_tray_icon");

	return 0;
}

extern "C" void qt4_docking_close()
{
	delete qt4_tray_icon;
	qt4_tray_icon = NULL;
}


Qt4TrayIcon::Qt4TrayIcon(QWidget *parent, const char *name)
{
	kdebugf();

	setIcon(QIcon(docking_manager->defaultPixmap()));

	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this, SLOT(setTrayPixmap(const QIcon&, const QString &)));
	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));

	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	show();
	setContextMenu(dockMenu);
	
	docking_manager->setDocked(true);

	kdebugf2();
}

Qt4TrayIcon::~Qt4TrayIcon()
{
	kdebugf();

	disconnect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this, SLOT(setTrayPixmap(const QIcon&, const QString &)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	disconnect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	docking_manager->setDocked(false);

	kdebugf2();
}

void Qt4TrayIcon::findTrayPosition(QPoint& pos)
{
	QRect rect = geometry();
	pos = QPoint(rect.x(), rect.y());
}

void Qt4TrayIcon::setTrayPixmap(const QIcon& pixmap, const QString &/*iconName*/)
{
	this->setIcon(pixmap);
}

void Qt4TrayIcon::setTrayMovie(const QMovie &movie)
{
	//QLabel::setMovie(movie);
}

void Qt4TrayIcon::setTrayTooltip(const QString& tooltip)
{
	this->setToolTip(tooltip);
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

