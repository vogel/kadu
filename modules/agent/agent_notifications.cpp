/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include "agent_notifications.h"
#include "agent.h"

#include "usergroup.h"
#include "config_file.h"
#include "misc/misc.h"
#include "debug.h"

NewUserFoundNotification::NewUserFoundNotification(UserListElements &users) : Notification("Agent/NewFound", dataPath("kadu/modules/data/agent/agent32.png"), users)
{
	kdebugf();

	setTitle("Agent");

	UserListElement user = *(users.constBegin());
	QString uin_str = user.ID("Gadu");
	setText(tr("User <b>%1</b> has you on his list!").arg(uin_str));

	addCallback(tr("Find user"), SLOT(callbackFind()));
	addCallback(tr("OK"), SLOT(callbackAccept()));
	setDefaultCallback(config_file.readNumEntry("Hints", "Event_Agent/NewFound_timeout", 10) * 1000, SLOT(callbackAccept()));

	kdebugf2();
}

void NewUserFoundNotification::callbackFind()
{
	kdebugf();

	agent->lockupUser(userListElements());
	close();

	kdebugf2();
}
