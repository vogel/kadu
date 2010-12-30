/*
 * %kadu copyright begin%
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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


#include "debug.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "imagelink.h"

extern "C" KADU_EXPORT int imagelink_init()
{
	kdebugf();
	imageLink = new ImageLink();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/imagelink.ui"));
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void imagelink_close()
{
	kdebugf();
	delete imageLink;
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/imagelink.ui"));
	imageLink = NULL;
	kdebugf2();
}
