/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "core/core.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/windows/buddy-data-window.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/windows/chat-data-window.h"
#include "gui/windows/kadu-window.h"

#include "edit-talkable-action.h"

EditTalkableAction::EditTalkableAction(QObject *parent) :
		ActionDescription(parent)
{
	// TODO: TypeChat | TypeUser or TypeTalkables
	setType(TypeGlobal);
	setName("editUserAction");
	setIcon(KaduIcon("x-office-address-book"));
	setText(tr("View Buddy Properties"));

	registerAction();
}

EditTalkableAction::~EditTalkableAction()
{
}

int EditTalkableAction::actionRole(ActionContext *context) const
{
	if (context->roles().contains(ChatRole))
		return ChatRole;
	if (context->roles().contains(BuddyRole) || context->roles().contains(ContactRole)) // we just use ownerBuddy
		return 1 == context->buddies().size() ? BuddyRole : ChatRole;
	return -1;
}

Chat EditTalkableAction::actionChat(ActionContext *context) const
{
	return context->chat();
}

Buddy EditTalkableAction::actionBuddy(ActionContext *context) const
{
	if (context->buddies().size())
		return context->buddies().toBuddy();
	else
		return context->contacts().toContact().ownerBuddy();
}

void EditTalkableAction::setChatActionTitleAndIcon(Action *action)
{
	action->setIcon(KaduIcon("x-office-address-book"));
	action->setText(QCoreApplication::translate("KaduWindowActions", "View Chat Properties"));
}

void EditTalkableAction::setBuddyActionTitleAndIcon(Action *action)
{
	const Buddy &buddy = actionBuddy(action->context());
	if (buddy.isAnonymous() && 1 == action->context()->buddies().count())
	{
		action->setIcon(KaduIcon("contact-new"));
		action->setText(QCoreApplication::translate("KaduWindowActions", "Add Buddy..."));
	}
	else
	{
		action->setIcon(KaduIcon("x-office-address-book"));
		action->setText(QCoreApplication::translate("KaduWindowActions", "View Buddy Properties"));
	}
}

void EditTalkableAction::updateChatActionState(Action *action)
{
	setChatActionTitleAndIcon(action);

	const Chat &chat = actionChat(action->context());
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	action->setEnabled(chat && (!chatType || (chatType->name() != "Contact" && !chat.display().isEmpty())));
}

void EditTalkableAction::updateBuddyActionState(Action *action)
{
	setBuddyActionTitleAndIcon(action);

	const Buddy &buddy = actionBuddy(action->context());
	if (!buddy)
		return;

	if (buddy == Core::instance()->myself())
		return;

	action->setEnabled(true);
}

void EditTalkableAction::actionInstanceCreated(Action *action)
{
	switch (actionRole(action->context()))
	{
		case ChatRole:
			setChatActionTitleAndIcon(action);
			break;
		case BuddyRole:
			setBuddyActionTitleAndIcon(action);
			break;
	}

	updateActionState(action);
}

void EditTalkableAction::updateActionState(Action *action)
{
	action->setEnabled(false);

	switch (actionRole(action->context()))
	{
		case ChatRole:
			updateChatActionState(action);
			break;
		case BuddyRole:
			updateBuddyActionState(action);
			break;
	}
}

void EditTalkableAction::chatActionTriggered(ActionContext *context)
{
	const Chat &chat = actionChat(context);
	if (!chat)
		return;

	Core::instance()->chatDataWindowRepository()->showChatWindow(chat);
}

void EditTalkableAction::buddyActionTriggered(ActionContext *context)
{
	Buddy buddy = actionBuddy(context);
	if (!buddy)
		return;
	if (buddy.isAnonymous())
		(new AddBuddyWindow(Core::instance()->kaduWindow(), buddy, true))->show();
	else
		Core::instance()->buddyDataWindowRepository()->showBuddyWindow(buddy);
}

void EditTalkableAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(toggled)

	trigger(context);
}

void EditTalkableAction::trigger(ActionContext *context)
{
	switch (actionRole(context))
	{
		case ChatRole:
			chatActionTriggered(context);
			break;
		case BuddyRole:
			buddyActionTriggered(context);
			break;
	}
}

#include "moc_edit-talkable-action.cpp"
