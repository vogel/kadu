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

#include "send-action.h"

#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/custom-input.h"

SendAction::SendAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"go-next"});
	setName(QStringLiteral("sendAction"));
	setText(tr("&Send"));
	setType(ActionDescription::TypeChat);
}

SendAction::~SendAction()
{
}

void SendAction::init()
{
	registerAction(actionsRegistry());
}

void SendAction::actionInstanceCreated(Action *action)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));
	connect(chatEditBox->chatWidget()->chat(), SIGNAL(connected()), action, SLOT(checkState()));
	connect(chatEditBox->chatWidget()->chat(), SIGNAL(disconnected()), action, SLOT(checkState()));
}

void SendAction::actionTriggered(QAction *action, bool)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	auto chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->sendMessage();
}

void SendAction::updateActionState(Action* action)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->context()->widget());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(chatEditBox->chatWidget()->chat().isConnected() && !chatEditBox->inputBox()->toPlainText().isEmpty());
}

#include "moc_send-action.cpp"
