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
#include <QtGui/QSpinBox>

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

// fullDesktop->width() - DesktopDockPixmap.size().width(), 1, 0,
// fullDesktop->height() - DesktopDockPixmap.size().height(), 1, 0,

	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this,  SLOT(setPixmap(const QIcon&, const QString &)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));

	connect(desktopDock, SIGNAL(dropped(const QPoint &)), this, SLOT(droppedOnDesktop(const QPoint &)));

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

	disconnect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this, SLOT(setPixmap(const QIcon&, const QString &)));
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

void DesktopDock::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("desktop_docking/transparent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("desktop_docking/color"), SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("desktop_docking/move"), SIGNAL(clicked()), desktopDock, SLOT(startMoving()));

	xSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("desktop_docking/x"));
	ySpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("desktop_docking/y"));
}

void DesktopDock::configurationUpdated()
{
	kdebugf();
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));

	desktopDock->move(pos);
	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
	{
// 		desktopDock->setPaletteBackgroundColor(Qt::transparent); /* ustawia kolor tla */
		desktopDock->close();
		desktopDock->show();
	}
	else
	{
// 		desktopDock->setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));	/* ustawia kolor tla */
		desktopDock->repaint();
	}
	kdebugf2();
}

void DesktopDock::setToolTip(const QString& statusText)
{
	desktopDock->setToolTip(statusText);
}

void DesktopDock::setPixmap(const QIcon& DockIcon, const QString & /*iconName*/)
{
	desktopDock->setPixmap(DockIcon.pixmap(128,128));
	desktopDock->repaint();
	desktopDock->setMask(desktopDock->pixmap()->createHeuristicMask(false));
}

void DesktopDock::setTrayMovie(const QMovie &movie)
{
	desktopDock->setMovie((QMovie *)&movie);
	desktopDock->repaint();
}

void DesktopDock::findTrayPosition(QPoint& DockPoint)	/* zwrocenie krawedzi ikony */
{
	DockPoint = desktopDock->mapToGlobal(QPoint(0,0));
}

void DesktopDock::droppedOnDesktop(const QPoint& pos) 	/* nacisniecie przycisku na ekranie - ustawienie ikonki i zapisanie ustawien */
{
	desktopDock->move(pos);
	desktopDock->update();
	desktopDock->show();

	QDesktopWidget *fullDesktop = QApplication::desktop();
	int posX, posY;

	if (pos.x() > fullDesktop->width() - desktopDock->pixmap()->width())
		posX = fullDesktop->width() - desktopDock->pixmap()->width();
	else
		posX = pos.x();

	if (pos.y() > fullDesktop->height() - desktopDock->pixmap()->height())
		posY = fullDesktop->height() - desktopDock->pixmap()->height();
	else
		posY = pos.y();

	xSpinBox->setValue(posX);
	ySpinBox->setValue(posY);
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

// 	config_file.addVariable("Desktop Dock", "DockingColor", w.paletteBackgroundColor());
	config_file.addVariable("Desktop Dock", "DockingTransparency", true);
	config_file.addVariable("Desktop Dock", "MoveInMenu", true);
	config_file.addVariable("Desktop Dock", "PositionX", 0);
	config_file.addVariable("Desktop Dock", "PositionY", 0);
}
