/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "icons/kadu-icon.h"

#include "plugins/docking/docking.h"

#include "desktop-dock-window.h"

DesktopDockWindow::DesktopDockWindow(QWidget *parent) :
		QLabel(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint),
		IsMoving(false)
{
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_MouseNoMask);
	setAutoFillBackground(true);
	setMouseTracking(true);

	setPixmap(DockingManager::instance()->defaultIcon().icon().pixmap(128, 128));
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
	QPoint pos(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Desktop Dock", "PositionX"), Application::instance()->configuration()->deprecatedApi()->readNumEntry("Desktop Dock", "PositionY"));
	move(pos);

	QPalette newPalette = palette();
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Desktop Dock", "DockingTransparency"))
	{
		newPalette.setColor(QPalette::Active, QPalette::Window, Qt::transparent);
		newPalette.setColor(QPalette::Inactive, QPalette::Window, Qt::transparent);
		if (pixmap())
			setMask(pixmap()->mask());
	}
	else
	{
		QColor color = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Desktop Dock", "DockingColor");
		newPalette.setColor(QPalette::Active, QPalette::Window, color);
		newPalette.setColor(QPalette::Inactive, QPalette::Window, color);
		clearMask();
	}

	setPalette(newPalette);
	update();
}

QPoint DesktopDockWindow::getCenterFromEvent(QMouseEvent* ev)
{
	return QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2);
}

void DesktopDockWindow::contextMenuEvent(QContextMenuEvent* ev)
{
	DockingManager::instance()->dockMenu()->popup(ev->globalPos());
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)
{
	if (IsMoving)
	{
		emit dropped(getCenterFromEvent(ev));
		IsMoving = false;
	}
	else
		DockingManager::instance()->trayMousePressEvent(ev);
}

void DesktopDockWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (IsMoving)
		move(getCenterFromEvent(ev));
}

void DesktopDockWindow::startMoving()
{
	IsMoving = true;
}

#include "moc_desktop-dock-window.cpp"
