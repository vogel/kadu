/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SCREENSHOT_ACTION_H
#define SCREENSHOT_ACTION_H

#include <QtGui/QAction>

#include "gui/actions/action-description.h"

class ChatWidget;

class ScreenshotAction : public ActionDescription
{
	Q_OBJECT

	ChatWidget * findChatWidget(QObject *obejct);

private slots:
	void takeStandardShotSlot(ChatWidget *chatWidget = 0);
	void takeShotWithChatWindowHiddenSlot();
	void takeWindowShotSlot();

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void actionTriggered(QAction *sender, bool toggled);
	virtual void updateActionState(Action *action);

public:
	explicit ScreenshotAction(QObject *parent);
	virtual ~ScreenshotAction();

};

#endif // SCREENSHOT_ACTION_H
