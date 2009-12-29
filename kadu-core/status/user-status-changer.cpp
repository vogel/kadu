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

void UserStatusChanger::changeStatus(Status &status)
{
	kdebugf();

	status = UserStatus;

	kdebugf2();
}

void UserStatusChanger::userStatusSet(const Status &status)
{
	kdebugf();

	UserStatus = status;
	emit statusChanged();

	kdebugf2();
}
