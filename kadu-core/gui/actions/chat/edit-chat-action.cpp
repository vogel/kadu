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

#include <QtGui/QInputDialog>

#include "chat/chat.h"
#include "gui/actions/action-context.h"

#include "edit-chat-action.h"

EditChatAction::EditChatAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("editChatAction");
	setIcon(KaduIcon("x-office-address-book"));
	setText(tr("Edit"));

	registerAction();
}

EditChatAction::~EditChatAction()
{
}

void EditChatAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(toggled)

	const Chat &chat = context->chat();
	if (!chat)
		return;

	bool ok;
	QString conferenceName = QInputDialog::getText(widget, tr("Name conference"),
	                                               tr("Please enter the name for this conference"),
	                                               QLineEdit::Normal, chat.display(), &ok);

	if (!ok)
		return;

	chat.setDisplay(conferenceName);
}
