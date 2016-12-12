/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-chat-action.h"

#include "chat/chat.h"
#include "actions/action-context.h"
#include "actions/action.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"

OpenChatAction::OpenChatAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"internet-group-chat"});
	setName(QStringLiteral("chatAction"));
	setText(tr("&Chat"));
	setType(ActionDescription::TypeUser);
}

OpenChatAction::~OpenChatAction()
{
}

void OpenChatAction::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void OpenChatAction::actionTriggered(QAction *sender, bool)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	m_chatWidgetManager->openChat(action->context()->chat(), OpenChatActivation::Activate);
}

void OpenChatAction::updateActionState(Action* action)
{
	disableNoChat(action);
}

#include "moc_open-chat-action.cpp"
