/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget.h"
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

void ScreenshotAction::actionInstanceCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox || !chatEditBox->chatWidget())
		return;

	QVariant chatWidgetData = (qlonglong)chatEditBox->chatWidget();
	action->setData(chatWidgetData);

	// not a menu
	if (action->context()->chat() != chatEditBox->actionContext()->chat())
		return;

	// no parents for menu as it is destroyed manually by Action class
	QMenu *menu = new QMenu();
	menu->addAction(tr("Simple Shot"), this, SLOT(takeStandardShotSlot()))->setData(chatWidgetData);
	menu->addAction(tr("With Chat Window Hidden"), this, SLOT(takeShotWithChatWindowHiddenSlot()))->setData(chatWidgetData);
#ifndef Q_OS_MAC
	menu->addAction(tr("Window Shot"), this, SLOT(takeWindowShotSlot()))->setData(chatWidgetData);
#endif
	action->setMenu(menu);
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

	const Account &account = action->context()->chat().chatAccount();
	if (!account)
		return;

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	action->setEnabled(protocol->chatImageService());
}

ChatWidget * ScreenshotAction::findChatWidget(QObject *object)
{
	QAction *action = qobject_cast<QAction *>(object);
	if (!action)
		return 0;
	return static_cast<ChatWidget *>((void*)(action->data().toLongLong()));
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

#include "moc_screenshot-action.cpp"
