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

#include <QtGui/QBitmap>

#include "configuration/configuration-file.h"

#include "modules/docking/docking.h"

#include "desktop-dock-window.h"
#include <QMouseEvent>

DesktopDockWindow::DesktopDockWindow(QWidget *parent) :
		QLabel(parent), IsMoving(false)
{
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_MouseNoMask);
	setMouseTracking(true);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

	QIcon desktopDockIcon = docking_manager->defaultPixmap();
	setPixmap(desktopDockIcon.pixmap(128, 128));
	resize(pixmap()->size());

	configurationUpdated();

	update();
	show();
}

DesktopDockWindow::~DesktopDockWindow()
{
}

void DesktopDockWindow::configurationUpdated()
{
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));
   	move(pos);

// 	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
// 		setPaletteBackgroundColor(Qt::transparent);
// 	else
// 		setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));
}

QPoint DesktopDockWindow::getCenterFromEvent(QMouseEvent* ev)
{
	return QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2);
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)
{
	if (IsMoving)
	{
		emit dropped(getCenterFromEvent(ev));
		IsMoving = false;
	}
	else
		docking_manager->trayMousePressEvent(ev);
}

void DesktopDockWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (IsMoving)
		move(getCenterFromEvent(ev));
}

void DesktopDockWindow::updateMask() // refreshing icon
{
	if (pixmap())
		setMask(pixmap()->createHeuristicMask(false));
}

void DesktopDockWindow::startMoving()
{
	IsMoving = true;
}
