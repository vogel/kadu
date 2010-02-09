/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include "misc/misc.h"
#include "debug.h"

#include "idle.h"

#include <QtCore/QTimer>

Idle *idle = 0;

extern "C" KADU_EXPORT int idle_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	idle = new Idle();

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void idle_close()
{
	kdebugf();

	delete idle;
	idle = 0;

	kdebugf2();
}
