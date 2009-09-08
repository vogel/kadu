/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "power_status_changer.h"

#include "status.h"
#include "debug.h"


PowerStatusChanger::PowerStatusChanger()
: StatusChanger(300), state(STATUS_NOT_SPECIFIED), index(0)
{
	kdebugf();
	status_changer_manager->registerStatusChanger(this);
	kdebugf2();
}

PowerStatusChanger::~PowerStatusChanger()
{
	kdebugf();
	status_changer_manager->unregisterStatusChanger(this);
	kdebugf2();
}

void PowerStatusChanger::changeStatus(UserStatus &status)
{
	kdebugf();
	switch(state)
	{
		case STATUS_ONLINE:
			status.setOnline(description);
			break;

		case STATUS_BUSY:
			status.setBusy(description);
			break;

		case STATUS_INVISIBLE:
			status.setInvisible(description);
			break;

		case STATUS_OFFLINE:
			status.setOffline(description);
			break;

		case STATUS_FFC:
			status.setTalkWithMe(description);
			break;

		case STATUS_DND:
			status.setDoNotDisturb(description);
			break;

		case STATUS_BY_INDEX:
			status.setIndex(index, description);

		default:
			kdebugm(KDEBUG_INFO, "Status state not specified\n");
			break;
	}
	state = STATUS_NOT_SPECIFIED;
	kdebugf2();
}

void PowerStatusChanger::setOnline(QString desc)
{
	kdebugf();
	setStatus(STATUS_ONLINE, desc);
	kdebugf2();
}

void PowerStatusChanger::setBusy(QString desc)
{
	kdebugf();
	setStatus(STATUS_BUSY, desc);
	kdebugf2();
}

void PowerStatusChanger::setInvisible(QString desc)
{
	kdebugf();
	setStatus(STATUS_INVISIBLE, desc);
	kdebugf2();
}

void PowerStatusChanger::setTalkWithMe(QString desc)
{
	kdebugf();
	setStatus(STATUS_FFC, desc);
	kdebugf2();
}

void PowerStatusChanger::setDoNotDisturb(QString desc)
{
	kdebugf();
	setStatus(STATUS_DND, desc);
	kdebugf2();
}

void PowerStatusChanger::setOffline(QString desc)
{
	kdebugf();
	setStatus(STATUS_OFFLINE, desc);
	kdebugf2();
}

void PowerStatusChanger::setStatus(StatusState passedState, QString passedDesc)
{
	kdebugf();
	state = passedState;
	description = passedDesc;
	emit statusChanged();
	kdebugf2();
}

void PowerStatusChanger::setIndex(int currStat, QString currDesc)
{
	kdebugf();
	state = STATUS_BY_INDEX;
	description = currDesc;
	emit statusChanged();
	kdebugf2();
}

