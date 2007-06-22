/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "hint_manager.h"
#include "debug.h"

/**
 * @ingroup hints
 * @{
 */
extern "C" int hints_init()
{
	kdebugf();

	hint_manager = new HintManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/hints.ui"), hint_manager);

	kdebugf2();
	return 0;
}

extern "C" void hints_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/hints.ui"), hint_manager);
	delete hint_manager;
	hint_manager = 0;

	kdebugf2();
}

/** @} */

