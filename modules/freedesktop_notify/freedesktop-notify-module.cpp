
/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/path-conversion.h"

#include "freedesktop-notify.h"

extern "C" KADU_EXPORT int freedesktop_notify_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	FreedesktopNotify::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/freedesktop_notify.ui"));
	MainConfigurationWindow::registerUiHandler(FreedesktopNotify::instance());

	return 0;
}

extern "C" KADU_EXPORT void freedesktop_notify_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/freedesktop_notify.ui"));
	MainConfigurationWindow::unregisterUiHandler(FreedesktopNotify::instance());

	FreedesktopNotify::destroyInstance();
}
