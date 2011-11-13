/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-context.h"
#include "gui/windows/chat-data-window.h"
#include "gui/windows/kadu-window.h"

#include "edit-chat-action.h"

EditChatAction::EditChatAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("editChatAction");
	setIcon(KaduIcon("x-office-address-book"));
	setText(tr("Edit Chat Data"));

	registerAction();
}

EditChatAction::~EditChatAction()
{
}

void EditChatAction::updateActionState(Action *action)
{
	const Chat &chat = action->context()->chat();
	action->setEnabled(chat && chat.contacts().size() > 1);
}

void EditChatAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(toggled)

	const Chat &chat = context->chat();
	if (!chat)
		return;

	ChatDataWindow::instance(chat, Core::instance()->kaduWindow())->show();
}
