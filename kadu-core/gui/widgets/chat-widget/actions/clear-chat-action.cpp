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

#include "clear-chat-action.h"

#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-edit-widget.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"

ClearChatAction::ClearChatAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"edit-clear"});
	setName(QStringLiteral("clearChatAction"));
	setText(tr("Clear Messages in Chat Window"));
	setType(ActionDescription::TypeChat);
}

ClearChatAction::~ClearChatAction()
{
}

void ClearChatAction::actionInstanceCreated(Action *action)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->context()->widget());
	if (!chatEditBox)
		return;

	connect(chatEditBox->chatWidget()->chatMessagesView(), SIGNAL(messagesUpdated()), action, SLOT(checkState()));
	updateActionState(action);
}

void ClearChatAction::actionTriggered(QAction *sender, bool)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	auto chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->clearChatWindow();
}

void ClearChatAction::updateActionState(Action* action)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->context()->widget());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(0 != chatEditBox->chatWidget()->chatMessagesView()->countMessages());
}

#include "moc_clear-chat-action.cpp"
