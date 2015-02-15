/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "core/core.h"
#include "gui/actions/action-context.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "leave-chat-action.h"

LeaveChatAction::LeaveChatAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("leaveChatAction");
	setIcon(KaduIcon("kadu_icons/block-buddy"));
	setText(tr("Leave"));

	registerAction();
}

LeaveChatAction::~LeaveChatAction()
{
}

void LeaveChatAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(toggled)

	auto chat = context->chat();
	if (!chat)
		return;

	auto account = chat.chatAccount();
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	auto chatService = protocol->chatService();
	if (!chatService)
		return;

	auto chatWidget = Core::instance()->chatWidgetRepository()->widgetForChat(chat);
	if (!chatWidget)
		return;

	auto dialog = MessageDialog::create(KaduIcon("dialog-warning"), tr("Kadu"),
		tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"),
		widget);
	dialog->addButton(QMessageBox::Yes, tr("Leave conference"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));
	if (!dialog->ask())
		return;

	chatService->leaveChat(chat);
	chatWidget->requestClose();
}

#include "moc_leave-chat-action.cpp"
