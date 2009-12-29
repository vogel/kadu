/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "user-status-changer.h"

UserStatusChanger::UserStatusChanger() :
		StatusChanger(0)
{
}

UserStatusChanger::~UserStatusChanger()
{
}

void UserStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	kdebugf();

	UserStatuses[container] = status;

	kdebugf2();
}

Status UserStatusChanger::status(StatusContainer *container)
{
	if (UserStatuses.contains(container))
		return UserStatuses[container];
	else
		return Status::null;
}

void UserStatusChanger::userStatusSet(StatusContainer *container, const Status &status)
{
	kdebugf();

	UserStatuses[container] = status;
	emit statusChanged(container);

	kdebugf2();
}
