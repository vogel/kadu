/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

class QLabel;
class QPushButton;

class ChatWidget;

class ScreenshotTaker : public QWidget
{
	Q_OBJECT

	ChatWidget *CurrentChatWidget;

	QLabel *IconLabel;
	QPushButton *CancelButton;

	bool Dragging;

	void createLayout();

private slots:
	void takeShot();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit ScreenshotTaker(ChatWidget *chatWidget);
	virtual ~ScreenshotTaker();

public slots:
	void takeStandardShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();

signals:
	void screenshotTaken(QPixmap screenshot, bool needsCrop);
	void screenshotNotTaken();

};

#endif // SCREENSHOT_TAKER_H
