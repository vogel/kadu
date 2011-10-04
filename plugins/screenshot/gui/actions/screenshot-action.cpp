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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "protocols/protocol.h"

#include "screenshot.h"

#include "screenshot-action.h"

ScreenshotAction::ScreenshotAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("ScreenShotAction");
	setIcon(KaduIcon("external_modules/screenshot-camera-photo"));
	setText(tr("ScreenShot"));

	registerAction();
}

ScreenshotAction::~ScreenshotAction()
{
}

QMenu * ScreenshotAction::menuForAction(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	Chat chat = action->chat();

	// not a menu
	if (!chatEditBox || chat != chatEditBox->chat())
		return 0;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return 0;

	// no parents for menu as it is destroyed manually by Action class
	QMenu *menu = new QMenu();
	menu->addAction(tr("Simple shot"), this, SLOT(takeStandardShotSlot()));
	menu->addAction(tr("With chat window hidden"), this, SLOT(takeShotWithChatWindowHiddenSlot()));
	menu->addAction(tr("Window shot"), this, SLOT(takeWindowShotSlot()));

	return menu;
}

void ScreenshotAction::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	takeStandardShotSlot(findChatWidget(sender));
}

void ScreenshotAction::updateActionState(Action *action)
{
	action->setEnabled(false);

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	Account account = action->chat().chatAccount();
	if (!account)
		return;

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	action->setEnabled(protocol->chatImageService());
}

ChatWidget * ScreenshotAction::findChatWidget(QObject *object)
{
	while (object) {
		ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(object);
		if (chatEditBox)
			return chatEditBox->chatWidget();

		object = object->parent();
	}

	return 0;
}

void ScreenshotAction::takeStandardShotSlot(ChatWidget *chatWidget)
{
	// in case of non-menu call
	if (!chatWidget)
		chatWidget = findChatWidget(sender());
	if (chatWidget)
		(new ScreenShot(chatWidget))->takeStandardShot();
}

void ScreenshotAction::takeShotWithChatWindowHiddenSlot()
{
	ChatWidget *chatWidget = findChatWidget(sender());
	if (chatWidget)
		(new ScreenShot(chatWidget))->takeShotWithChatWindowHidden();
}

void ScreenshotAction::takeWindowShotSlot()
{
	ChatWidget *chatWidget = findChatWidget(sender());
	if (chatWidget)
		(new ScreenShot(chatWidget))->takeWindowShot();
}
