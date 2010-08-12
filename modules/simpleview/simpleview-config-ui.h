/*
 * %kadu copyright begin%
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
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
#ifndef SIMPLEVIEW_CONFIG_UI_H
#define SIMPLEVIEW_CONFIG_UI_H

#include <QtCore/QObject>


#include "gui/windows/main-configuration-window.h"

class SimpleViewConfigUi :
	public ConfigurationUiHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(SimpleViewConfigUi)

	static SimpleViewConfigUi *Instance;

	SimpleViewConfigUi();
	virtual ~SimpleViewConfigUi();

	void createDefaultConfiguration();

	/* Keep this code for further use:
private slots:
	void mainConfigurationWindowDestroyed();
	 */

public:
	static void createInstance();
	static void destroyInstance();
	static SimpleViewConfigUi *instance(){return Instance; }

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

#endif
