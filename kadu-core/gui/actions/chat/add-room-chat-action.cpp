/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "gui/actions/action.h"

#include "gui/windows/add-room-chat-window.h"

#include "add-room-chat-action.h"

AddRoomChatAction::AddRoomChatAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeGlobal);
	setName("addRoomChatAction");
	setText(tr("Join Room..."));

	registerAction();

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));

	updateAddChatMenuItem();
}

AddRoomChatAction::~AddRoomChatAction()
{
}

void AddRoomChatAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(context)
	Q_UNUSED(toggled)

	(new AddRoomChatWindow(widget))->show();
}

void AddRoomChatAction::updateAddChatMenuItem()
{
	bool isRoomChatSupported = false;

	foreach (const Account &account, AccountManager::instance()->items())
	{
		if (account.protocolName() == "jabber")
			isRoomChatSupported = true;
	}

	foreach (Action *action, actions())
	{
		action->setVisible(isRoomChatSupported);
	}
}

#include "moc_add-room-chat-action.cpp"
