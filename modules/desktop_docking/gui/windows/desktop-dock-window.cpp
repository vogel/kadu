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

#include "docking/docking.h"

#include "desktop-dock-window.h"

DesktopDockWindow::DesktopDockWindow(QWidget *parent) :
		QLabel(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint),
		isMoving(false)
{
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_MouseNoMask);

	QIcon desktopDockIcon = docking_manager->defaultPixmap();
	configurationUpdated();
	setMouseTracking(true);

	setPixmap(desktopDockIcon.pixmap(128, 128));
	resize(pixmap()->size());

	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
		setPaletteBackgroundColor(Qt::transparent);
	else
		setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));

	update();
	show();
}

DesktopDockWindow::~DesktopDockWindow()
{
}

void DesktopDockWindow::configurationUpdated()
{
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));
	if (!config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
		setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));
   	move(pos);
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)
{
	if (!isMoving)
		docking_manager->trayMousePressEvent(ev);
	else
	{
		emit dropped(QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2));
		isMoving = false;
	}
}

void DesktopDockWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (isMoving)
		move(QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2));
}

void DesktopDockWindow::updateMask()		/* to zalatwia automatyczne odswiezenie ikony - jak to dziala nie pytac (wazne ze dziala) */
{
	if (pixmap())
		setMask(pixmap()->createHeuristicMask(false));
}

void DesktopDockWindow::startMoving()	/* rozpoczynamy wojaze po ekranie */
{
	isMoving = true;
}
