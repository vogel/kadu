/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef DESKTOP_DOCK_CONFIGURATION_UI_HANDLER_H
#define DESKTOP_DOCK_CONFIGURATION_UI_HANDLER_H

#include "gui/windows/main-configuration-window.h"

class QSpinBox;

class DesktopDockConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(DesktopDockConfigurationUiHandler)

	static DesktopDockConfigurationUiHandler * Instance;

	QSpinBox *XSpinBox;
	QSpinBox *YSpinBox;

	DesktopDockConfigurationUiHandler();
	virtual ~DesktopDockConfigurationUiHandler();

private slots:
	void dockWindowDropped(const QPoint &pos);

public:
	static void createInstance();
	static void destroyInstance();
	static DesktopDockConfigurationUiHandler * instance();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // DESKTOP_DOCK_CONFIGURATION_UI_HANDLER_H
