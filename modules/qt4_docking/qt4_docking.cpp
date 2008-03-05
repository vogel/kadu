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

	show();
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

	docking_manager->setDocked(false);

	kdebugf2();
}

void Qt4TrayIcon::findTrayPosition(QPoint& pos)
{
	//QRect rect = geometry();
	//pos = QPoint(rect.x(), rect.y());
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

void Qt4TrayIcon::mousePressEvent(QMouseEvent * e)
{
	docking_manager->trayMousePressEvent(e);
}

Qt4TrayIcon* qt4_tray_icon = NULL;

/** @} */

