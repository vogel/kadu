/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>

#include "window-manager.h"

WindowManager *WindowManager::Instance = 0;

WindowManager * WindowManager::instance()
{
	if (!Instance)
		Instance = new WindowManager();

	return Instance;
}

WindowManager::WindowManager() : QObject()
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::moveToWorkspaceCenter(QWidget *window)
{
	const QSize windowSizeHint = window->sizeHint();
	const QRect workspaceGeometry = qApp->desktop()->availableGeometry(window);

	int xPosition = (workspaceGeometry.width() - windowSizeHint.width()) / 2;
	int yPosition = (workspaceGeometry.height() - windowSizeHint.height()) / 2;

	moveToPosition(window, QPoint(xPosition, yPosition));
}

void WindowManager::moveToPosition(QWidget *window, const QPoint &position)
{
	window->move(position);
	ensureWholeVisible(window);
}

void WindowManager::ensureWholeVisible(QWidget *window)
{
	const QSize windowSizeHint = window->sizeHint();
	const QRect workspaceGeometry = qApp->desktop()->availableGeometry(window);

	QPoint windowPosition = window->geometry().topLeft();
	if (windowPosition.x() < workspaceGeometry.left())
		windowPosition.setX(workspaceGeometry.left());
	if (windowPosition.y() < workspaceGeometry.top())
		windowPosition.setY(workspaceGeometry.top());
	if (windowPosition.x() + windowSizeHint.width() > workspaceGeometry.right())
		windowPosition.setX(workspaceGeometry.right() - windowSizeHint.width());
	if (windowPosition.y() + windowSizeHint.height() > workspaceGeometry.bottom())
		windowPosition.setY(workspaceGeometry.bottom() - windowSizeHint.height());

	window->move(windowPosition);
}
