/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/path-conversion.h"
#include "debug.h"
#include "hint_manager.h"
#include "hints-configuration-ui-handler.h"

/**
 * @ingroup hints
 * @{
 */
extern "C" KADU_EXPORT int hints_init(bool firstLoad)
{
	kdebugf();

	hint_manager = new HintManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/hints.ui"));
	MainConfigurationWindow::registerUiHandler(hint_manager->uiHandler());

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void hints_close()
{
	kdebugf();

//	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/hints.ui"));
	MainConfigurationWindow::unregisterUiHandler(hint_manager->uiHandler());
	
	delete hint_manager;
	hint_manager = 0;

	kdebugf2();
}

/** @} */

