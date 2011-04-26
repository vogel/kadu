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
#include <QtGui/QMovie>

#include "configuration/configuration-aware-object.h"

#include "plugins/docking/docker.h"

class QAction;

class DesktopDockWindow;
class KaduIcon;

class DesktopDock : public QObject, public Docker, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DesktopDock)

	static DesktopDock *Instance;

	DesktopDockWindow *DockWindow;

	QAction *MoveMenuAction;

	explicit DesktopDock(QObject *parent = 0);
	virtual ~DesktopDock();

	void createDefaultConfiguration();

	void createMenu();
	void destroyMenu();

private slots:
	void updateMenu(bool);

protected:
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();
	static DesktopDock * instance();

	DesktopDockWindow * dockWindow() { return DockWindow; }

    virtual void changeTrayIcon(const KaduIcon &icon);
    virtual void changeTrayMovie(const QString &moviePath);
    virtual void changeTrayTooltip(const QString &tooltip);
    virtual QPoint trayPosition();

};

#endif // DESKTOP_DOCK_H
