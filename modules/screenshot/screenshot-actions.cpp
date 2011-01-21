/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "debug.h"

#include "screenshot.h"

#include "screenshot-actions.h"

ScreenshotActions *ScreenshotActions::Instance = 0;

static void disableNoChatImageService(Action *action)
{
	action->setEnabled(false);

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
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

void ScreenshotActions::registerActions()
{
	if (Instance)
		return;

	Instance = new ScreenshotActions();
}

void ScreenshotActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

ScreenshotActions * ScreenshotActions::instance()
{
	return Instance;
}

ScreenshotActions::ScreenshotActions() :
		CurrentChatWidget(0)
{
	// Chat toolbar button
	ScreenShotActionDescription = new ActionDescription(this,
			ActionDescription::TypeChat, "ScreenShotAction",
			this, SLOT(screenshotActionActivated(QAction *, bool)),
			"external_modules/screenshot-camera-photo",
			tr("ScreenShot"), false,
			disableNoChatImageService
	);

	createMenu();
}

ScreenshotActions::~ScreenshotActions()
{
	delete Menu;
	Menu = 0;
}

void ScreenshotActions::createMenu()
{
	Menu = new QMenu();
	Menu->addAction(tr("Simple shot"), this, SLOT(takeStandardShotSlot()));
	Menu->addAction(tr("With chat window hidden"), this, SLOT(takeShotWithChatWindowHiddenSlot()));
	Menu->addAction(tr("Window shot"), this, SLOT(takeWindowShotSlot()));
}

void ScreenshotActions::screenshotActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	CurrentChatWidget = chatEditBox->chatWidget();
	if (CurrentChatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		QWidget *widget = widgets[widgets.size() - 1];
		Menu->popup(widget->mapToGlobal(QPoint(0, widget->height())));
	}
}

void ScreenshotActions::takeStandardShotSlot()
{
	if (CurrentChatWidget)
		(new ScreenShot(CurrentChatWidget))->takeStandardShot();
}

void ScreenshotActions::takeShotWithChatWindowHiddenSlot()
{
	if (CurrentChatWidget)
		(new ScreenShot(CurrentChatWidget))->takeShotWithChatWindowHidden();
}

void ScreenshotActions::takeWindowShotSlot()
{
	if (CurrentChatWidget)
		(new ScreenShot(CurrentChatWidget))->takeWindowShot();
}
