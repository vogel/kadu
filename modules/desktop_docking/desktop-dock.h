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

#ifndef DESKTOP_DOCK_H
#define DESKTOP_DOCK_H

#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtGui/QMovie>

class QAction;

class DesktopDockWindow;

class DesktopDock : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DesktopDock)

	static DesktopDock *Instance;

	DesktopDockWindow *desktopDock;

	QAction *menuPos;
	QAction *separatorPos;

	void createDefaultConfiguration();

	explicit DesktopDock(QObject *parent = 0);
	virtual ~DesktopDock();

private slots:
	void setToolTip(const QString &statusText);
	void setPixmap(const QIcon &DockIcon);
	void setTrayMovie(const QString &movie);
	void findTrayPosition(QPoint &DockPoint);
	void updateMenu(bool);

public:
	static void createInstance();
	static void destroyInstance();
	static DesktopDock * instance();

	DesktopDockWindow * dockWindow() { return desktopDock; }

};

#endif // DESKTOP_DOCK_H
