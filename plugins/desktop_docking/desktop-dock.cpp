/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "icons/kadu-icon.h"
#include "debug.h"

#include "plugins/docking/docking.h"

#include "gui/windows/desktop-dock-window.h"

#include <gui/widgets/configuration/configuration-widget.h>
#include "desktop-dock.h"

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
		QObject(parent)
{
	kdebugf();

	createDefaultConfiguration();

	DockWindow = new DesktopDockWindow();
	MoveMenuAction = new QAction(tr("Move"), DockWindow);
	connect(MoveMenuAction, SIGNAL(triggered()), DockWindow, SLOT(startMoving()));

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
		createMenu();

	kdebugf2();
}

DesktopDock::~DesktopDock()
{
	kdebugf();

	destroyMenu();

	delete MoveMenuAction;
	MoveMenuAction = 0;

	delete DockWindow;
	DockWindow = 0;

	kdebugf2();
}

void DesktopDock::createMenu()
{
	DockingManager::instance()->registerModuleAction(MoveMenuAction);
}

void DesktopDock::destroyMenu()
{
	DockingManager::instance()->unregisterModuleAction(MoveMenuAction);
}

void DesktopDock::changeTrayIcon(const KaduIcon &icon)
{
	DockWindow->setPixmap(icon.icon().pixmap(128,128));
	DockWindow->repaint();
	DockWindow->setMask(DockWindow->pixmap()->createHeuristicMask(false));
}

void DesktopDock::changeTrayMovie(const QString &moviePath)
{
	Q_UNUSED(moviePath)

//	hmm
// 	desktopDock->setMovie((QMovie *)&movie);
	DockWindow->repaint();
}

void DesktopDock::changeTrayTooltip(const QString &tooltip)
{
	DockWindow->setToolTip(tooltip);
}

QPoint DesktopDock::trayPosition()
{
	return DockWindow->mapToGlobal(QPoint(0,0));
}

void DesktopDock::updateMenu(bool b)
{
	if (b)
		createMenu();
	else
		destroyMenu();
}

void DesktopDock::configurationUpdated()
{
	updateMenu(config_file.readBoolEntry("Desktop Dock", "MoveInMenu"));
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

#include "moc_desktop-dock.cpp"
