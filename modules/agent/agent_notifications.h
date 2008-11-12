/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AGENT_NOTIFICATIONS_H
#define AGENT_NOTIFICATIONS_H

#include "agent.h"

#include "../notify/notification.h"

class Agent;
class UserListElements;

class NewUserFoundNotification : public Notification
{
	Q_OBJECT

public:
	NewUserFoundNotification(UserListElements &users);
	~NewUserFoundNotification() { clearCallbacks(); }

public slots:
	void callbackFind();
};

#endif
