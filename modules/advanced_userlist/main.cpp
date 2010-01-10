/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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
#include "main_configuration_window.h"

#include "advanced_userlist.h"

static AdvancedUserList *advanced_userlist;

extern "C" KADU_EXPORT int advanced_userlist_init(bool firstLoad)
{
	kdebugf();

	advanced_userlist = new AdvancedUserList();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/advanced_userlist.ui"), advanced_userlist);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void advanced_userlist_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/advanced_userlist.ui"), advanced_userlist);
	delete advanced_userlist;

	kdebugf2();
}
