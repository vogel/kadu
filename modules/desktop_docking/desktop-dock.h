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

#include "gui/windows/main-configuration-window.h"

class QSpinBox;
class DesktopDockWindow;

class DesktopDock : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DesktopDock)

	static DesktopDock *Instance;

	DesktopDockWindow *desktopDock;
	QSpinBox *xSpinBox;
	QSpinBox *ySpinBox;

	QAction *menuPos;
	QAction *separatorPos;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

private slots:
	void setToolTip(const QString &statusText);
	void setPixmap(const QIcon &DockIcon, const QString &iconName);
	void setTrayMovie(const QMovie &movie);
	void findTrayPosition(QPoint &DockPoint);
	void droppedOnDesktop(const QPoint &);
	void updateMenu(bool);

public:
	static void createInstance();
	static void destroyInstance();
	static DesktopDock * instance();

	explicit DesktopDock(QObject *parent = 0);
	virtual ~DesktopDock();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // DESKTOP_DOCK_H
