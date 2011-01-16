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

#ifndef SCREENSHOT_ACTIONS_H
#define SCREENSHOT_ACTIONS_H

#include <QtCore/QObject>

class QAction;
class QMenu;

class ActionDescription;
class ChatWidget;

class ScreenshotActions : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ScreenshotActions)

	static ScreenshotActions *Instance;

	ActionDescription *ScreenShotActionDescription;
	QMenu *Menu;

	ChatWidget *CurrentChatWidget;

	ScreenshotActions();
	virtual ~ScreenshotActions();

	void createMenu();

private slots:
	void screenshotActionActivated(QAction *sender, bool toggled);

	void takeStandardShotSlot();
	void takeShotWithChatWindowHiddenSlot();
	void takeWindowShotSlot();

public:
	static void registerActions();
	static void unregisterActions();

	static ScreenshotActions * instance();

};

#endif // SCREENSHOT_ACTIONS_H
