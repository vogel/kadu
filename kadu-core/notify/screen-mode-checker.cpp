/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QTimer>

#include "screen-mode-checker.h"

#define FULLSCREENCHECKTIMER_INTERVAL 2000 /*ms*/

ScreenModeChecker::ScreenModeChecker()
		: FullScreenCheckTimer(0), InFullScreen(false)
{
}

ScreenModeChecker::~ScreenModeChecker()
{
	disable();
}

void ScreenModeChecker::enable()
{
	if (isDummy())
		return;

	if (!FullScreenCheckTimer)
		FullScreenCheckTimer = new QTimer(this);

	FullScreenCheckTimer->setInterval(FULLSCREENCHECKTIMER_INTERVAL);
	connect(FullScreenCheckTimer, SIGNAL(timeout()), this, SLOT(checkFullScreen()));
	FullScreenCheckTimer->start();
}

void ScreenModeChecker::disable()
{
	if (!FullScreenCheckTimer)
		return;

	FullScreenCheckTimer->stop();
	disconnect(FullScreenCheckTimer, 0, this, 0);
	FullScreenCheckTimer->deleteLater();
	FullScreenCheckTimer = 0;
}

void ScreenModeChecker::checkFullScreen()
{
	bool inFullScreenNow = isFullscreenAppActive() && !isScreensaverActive();

	if (InFullScreen != inFullScreenNow)
	{
		InFullScreen = inFullScreenNow;
		emit fullscreenToggled(InFullScreen);
	}
}


#include "moc_screen-mode-checker.cpp"
