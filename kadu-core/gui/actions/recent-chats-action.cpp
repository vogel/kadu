/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/recent-chats-menu.h"

#include "recent-chats-action.h"

RecentChatsAction::RecentChatsAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("openRecentChatsAction");
	setIcon(KaduIcon("internet-group-chat"));
	setText(tr("Recent Chats"));

	RecentChatsMenuInstance = new RecentChatsMenu();
	connect(RecentChatsMenuInstance, SIGNAL(triggered(QAction *)),
		this, SLOT(openRecentChats(QAction *)));
}

RecentChatsAction::~RecentChatsAction()
{
	delete RecentChatsMenuInstance;
	RecentChatsMenuInstance = 0;
}

void RecentChatsAction::actionInstanceCreated(Action *action)
{
	action->setEnabled(false);
	action->setMenu(RecentChatsMenuInstance);
	connect(RecentChatsMenuInstance, SIGNAL(chatsListAvailable(bool)),
		action, SLOT(setEnabled(bool)));
}

void RecentChatsAction::openRecentChats(QAction *action)
{
	ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(action->data().value<Chat>(), true);
	if (chatWidget)
		chatWidget->activate();
}
