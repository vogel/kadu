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
		QObject(parent), MoveMenuAction(0), SeparatorAction(0)
{
	kdebugf();

	createDefaultConfiguration();

	DockWindow = new DesktopDockWindow();

	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&)), this,  SLOT(setPixmap(const QIcon&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(QString)), this, SLOT(setTrayMovie(QString)));

	docking_manager->setDocked(true);

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
		createMenu();

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

	destroyMenu();

	delete DockWindow;
	DockWindow = 0;

	kdebugf2();
}

void DesktopDock::createMenu()
{
	if (!SeparatorAction && !MoveMenuAction)
	{
		SeparatorAction = docking_manager->dockMenu()->addSeparator();
		MoveMenuAction = docking_manager->dockMenu()->addAction(tr("Move"), DockWindow, SLOT(startMoving()));
	}
}

void DesktopDock::destroyMenu()
{
	if (MoveMenuAction)
	{
		docking_manager->dockMenu()->removeAction(MoveMenuAction);
		delete MoveMenuAction;
		MoveMenuAction = 0;
	}

	if (SeparatorAction)
	{
		docking_manager->dockMenu()->removeAction(SeparatorAction);
		delete SeparatorAction;
		SeparatorAction = 0;
	}
}

void DesktopDock::setToolTip(const QString& statusText)
{
	DockWindow->setToolTip(statusText);
}

void DesktopDock::setPixmap(const QIcon& DockIcon)
{
	DockWindow->setPixmap(DockIcon.pixmap(128,128));
	DockWindow->repaint();
	DockWindow->setMask(DockWindow->pixmap()->createHeuristicMask(false));
}

void DesktopDock::setTrayMovie(const QString &movie)
{
	Q_UNUSED(movie)

//	hmm
// 	desktopDock->setMovie((QMovie *)&movie);
	DockWindow->repaint();
}

void DesktopDock::findTrayPosition(QPoint& DockPoint)	/* zwrocenie krawedzi ikony */
{
	DockPoint = DockWindow->mapToGlobal(QPoint(0,0));
}

void DesktopDock::updateMenu(bool b)
{
	if (b)
		createMenu();
	else
		destroyMenu();
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
