/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SCREEN_MODE_CHECKER_H
#define SCREEN_MODE_CHECKER_H

#include <QtCore/QObject>

class QTimer;

class ScreenModeChecker : public QObject
{
	Q_OBJECT

	QTimer *FullScreenCheckTimer;
	bool InFullScreen;

private slots:
	void checkFullScreen();

public:
	ScreenModeChecker();
	virtual ~ScreenModeChecker();

	void enable();
	void disable();

	// Do not call these methods in c-tor/d-tor!
	virtual bool isFullscreenAppActive() { return false; }
	virtual bool isScreensaverActive() { return false; }
	virtual bool isDummy() { return true; }

signals:
	void fullscreenToggled(bool inFullscreen);
};

#endif // SCREEN_MODE_CHECKER_H
