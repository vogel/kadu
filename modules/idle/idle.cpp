/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"
#include "debug.h"

#include "idle.h"

#include <QtCore/QTimer>

Idle *idle = 0;

extern "C" KADU_EXPORT int idle_init(bool firstLoad)
{
	kdebugf();

	idle = new Idle();
	
	QTimer *timer = new QTimer(idle);
	QObject::connect(timer, SIGNAL(timeout()), idle, SLOT(test()));
	timer->start(1000);

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
