/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef RECENT_CHATS_ACTION_H
#define RECENT_CHATS_ACTION_H

#include "gui/actions/action-description.h"

class RecentChatsMenu;

class RecentChatsAction : public ActionDescription
{
	Q_OBJECT

	RecentChatsMenu *RecentChatsMenuInstance;

private slots:
	void openRecentChats(QAction *action);
	
protected:
	virtual void actionInstanceCreated(Action *action);

public:
	explicit RecentChatsAction(QObject *parent);
	virtual ~RecentChatsAction();

};

#endif // RECENT_CHATS_ACTION_H
