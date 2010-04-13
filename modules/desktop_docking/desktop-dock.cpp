/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMenu>
#include <QtGui/QMovie>

#include "configuration/configuration-file.h"
#include "debug.h"

#include "modules/docking/docking.h"

#include "gui/windows/desktop-dock-window.h"

#include "desktop-dock.h"
#include <gui/widgets/configuration/configuration-widget.h>

DesktopDock * DesktopDock::Instance = 0;

void DesktopDock::createInstance()
{
	if (!Instance)
		Instance = new DesktopDock();
}

void DesktopDock::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

DesktopDock * DesktopDock::instance()
{
	return Instance;
}


DesktopDock::DesktopDock(QObject *parent) :
		QObject(parent), menuPos(0), separatorPos(0)
{
	kdebugf();

	createDefaultConfiguration();

	desktopDock = new DesktopDockWindow();

	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&)), this,  SLOT(setPixmap(const QIcon&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(QString)), this, SLOT(setTrayMovie(QString)));

	docking_manager->setDocked(true);

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		separatorPos = docking_manager->dockMenu()->addSeparator();
		menuPos = docking_manager->dockMenu()->addAction(tr("Move"), desktopDock, SLOT(startMoving()));
	}

	kdebugf2();
}

DesktopDock::~DesktopDock()
{
	kdebugf();

	disconnect(docking_manager, SIGNAL(trayMovieChanged(const QString &)), this, SLOT(setTrayMovie(const QString &)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&)), this, SLOT(setPixmap(const QIcon&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	docking_manager->setDocked(false);

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		docking_manager->dockMenu()->removeAction(menuPos);
		docking_manager->dockMenu()->removeAction(separatorPos);
	}

	delete menuPos;
	menuPos = 0;
	delete separatorPos;
	separatorPos = 0;

	delete desktopDock;
	desktopDock = 0;

	kdebugf2();
}

void DesktopDock::setToolTip(const QString& statusText)
{
	desktopDock->setToolTip(statusText);
}

void DesktopDock::setPixmap(const QIcon& DockIcon)
{
	desktopDock->setPixmap(DockIcon.pixmap(128,128));
	desktopDock->repaint();
	desktopDock->setMask(desktopDock->pixmap()->createHeuristicMask(false));
}

void DesktopDock::setTrayMovie(const QString &movie)
{
	Q_UNUSED(movie)

//	hmm
// 	desktopDock->setMovie((QMovie *)&movie);
	desktopDock->repaint();
}

void DesktopDock::findTrayPosition(QPoint& DockPoint)	/* zwrocenie krawedzi ikony */
{
	DockPoint = desktopDock->mapToGlobal(QPoint(0,0));
}

void DesktopDock::updateMenu(bool b)
{
	if (b)
	{
		separatorPos = docking_manager->dockMenu()->addSeparator();
		menuPos = docking_manager->dockMenu()->addAction(tr("Move"), desktopDock, SLOT(startMoving()));
	}
	else
	{
		docking_manager->dockMenu()->removeAction(menuPos);
		docking_manager->dockMenu()->removeAction(separatorPos);

		delete menuPos;
		menuPos = 0;
		delete separatorPos;
		separatorPos = 0;
	}
}

void DesktopDock::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Desktop Dock", "DockingColor", w.palette().color(QPalette::Active, QPalette::Window));
	config_file.addVariable("Desktop Dock", "DockingTransparency", true);
	config_file.addVariable("Desktop Dock", "MoveInMenu", true);
	config_file.addVariable("Desktop Dock", "PositionX", 0);
	config_file.addVariable("Desktop Dock", "PositionY", 0);
}
