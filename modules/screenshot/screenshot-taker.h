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

#ifndef SCREENSHOT_TAKER_H
#define SCREENSHOT_TAKER_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

class ChatWidget;

class ScreenshotTaker : public QObject
{
	Q_OBJECT

	ChatWidget *CurrentChatWidget;

private slots:
	void takeShot();

public:
	explicit ScreenshotTaker(ChatWidget *chatWidget, QObject *parent);
	virtual ~ScreenshotTaker();

public slots:
	void takeStandardShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();

signals:
	void screenshotTaken(QPixmap screenshot);

};

#endif // SCREENSHOT_TAKER_H
