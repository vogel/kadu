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

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "debug.h"

#include "desktopdock.h"

extern "C" KADU_EXPORT int desktop_docking_init(bool firstLoad)
{
	kdebugf();

	desktop_dock = new DesktopDock();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/desktop_docking.ui"), desktop_dock);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void desktop_docking_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/desktop_docking.ui"), desktop_dock);
	delete desktop_dock;
	desktop_dock = 0;

	kdebugf2();
}
