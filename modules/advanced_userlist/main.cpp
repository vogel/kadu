/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "main_configuration_window.h"

#include "advanced_userlist.h"

static AdvancedUserList *advanced_userlist;

extern "C" int advanced_userlist_init()
{
	kdebugf();

	advanced_userlist = new AdvancedUserList();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/advanced_userlist.ui"), advanced_userlist);

	kdebugf2();
	return 0;
}

extern "C" void advanced_userlist_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/advanced_userlist.ui"), advanced_userlist);
	delete advanced_userlist;

	kdebugf2();
}
