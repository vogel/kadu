/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QApplication>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "core/core.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/windows/buddy-delete-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"

#include "delete-talkable-action.h"

DeleteTalkableAction::DeleteTalkableAction(QObject *parent) :
		ActionDescription(parent)
{
	// TODO: TypeChat | TypeUser or TypeTalkables
	setType(TypeUser);
	setName("deleteUsersAction");
	setIcon(KaduIcon("edit-delete"));
	setShortcut("kadu_deleteuser");
	setText(tr("Delete Buddy"));

	registerAction();
}

DeleteTalkableAction::~DeleteTalkableAction()
{
}

int DeleteTalkableAction::actionRole(ActionContext *context) const
{
	if (context->roles().contains(ContactRole)) // we wont allow deleting contacts
		return -1;
	if (context->roles().contains(ChatRole))
		return ChatRole;
	if (context->roles().contains(BuddyRole))
		return BuddyRole;
	return -1;
}

Chat DeleteTalkableAction::actionChat(ActionContext *context) const
{
	return context->chat();
}

Buddy DeleteTalkableAction::actionBuddy(ActionContext *context) const
{
	if (context->buddies().size())
		return context->buddies().toBuddy();
	else
		return context->contacts().toContact().ownerBuddy();
}

void DeleteTalkableAction::setChatActionTitleAndIcon(Action *action)
{
	action->setText(QCoreApplication::translate("KaduWindowActions", "Delete Chat"));
}

void DeleteTalkableAction::setBuddyActionTitleAndIcon(Action *action)
{
	action->setText(QCoreApplication::translate("KaduWindowActions", "Delete Buddy"));
}

void DeleteTalkableAction::updateChatActionState(Action *action)
{
	setChatActionTitleAndIcon(action);

	const Chat &chat = actionChat(action->context());
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	action->setEnabled(chat && (!chatType || (chatType->name() != "Contact" && !chat.display().isEmpty())));
}

void DeleteTalkableAction::updateBuddyActionState(Action *action)
{
	setBuddyActionTitleAndIcon(action);

	const BuddySet &buddies = action->context()->buddies();
	if (buddies.isEmpty() || buddies.contains(Core::instance()->myself()))
		return;

	action->setEnabled(true);
}

void DeleteTalkableAction::actionInstanceCreated(Action *action)
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

void DeleteTalkableAction::updateActionState(Action *action)
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

void DeleteTalkableAction::chatActionTriggered(ActionContext *context)
{
	const Chat &chat = actionChat(context);
	if (!chat)
		return;

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-warning"), tr("Delete Chat"),
	                        tr("<b>%1</b> chat will be deleted.<br/>Are you sure?").arg(chat.display()));
	dialog->addButton(QMessageBox::Yes, tr("Delete chat"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (dialog->ask())
		chat.setDisplay(QString());
}

void DeleteTalkableAction::buddyActionTriggered(ActionContext *context)
{
	BuddySet buddySet = context->buddies();
	if (buddySet.empty())
		return;

	BuddyDeleteWindow *deleteWindow = new BuddyDeleteWindow(buddySet);
	deleteWindow->show();
}

void DeleteTalkableAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(toggled)

	trigger(context);
}

void DeleteTalkableAction::trigger(ActionContext *context)
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

#include "moc_delete-talkable-action.cpp"
