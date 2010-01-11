/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "power_status_changer.h"

#include "status.h"
#include "debug.h"


PowerStatusChanger::PowerStatusChanger()
: StatusChanger(300), state(STATUS_NOT_SPECIFIED), index(0)
{
	kdebugf();
	StatusChangerManager::instance()->registerStatusChanger(this);
	kdebugf2();
}

PowerStatusChanger::~PowerStatusChanger()
{
	kdebugf();
	StatusChangerManager::instance()->unregisterStatusChanger(this);
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

