/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
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
	Q_UNUSED(firstLoad)

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

