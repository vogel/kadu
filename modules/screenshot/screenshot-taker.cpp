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

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include "gui/widgets/chat-widget.h"
#include "pixmap-grabber.h"

#include "screenshot-taker.h"

ScreenshotTaker::ScreenshotTaker(ChatWidget *chatWidget, QWidget *parent) :
		QWidget(parent), CurrentChatWidget(chatWidget)
{
	setMouseTracking(true);
}

ScreenshotTaker::~ScreenshotTaker()
{
}

void ScreenshotTaker::takeStandardShot()
{
	if (CurrentChatWidget)
		CurrentChatWidget->update();

	QTimer::singleShot(1000, this, SLOT(takeShot()));
}

void ScreenshotTaker::takeShotWithChatWindowHidden()
{
	CurrentChatWidget->window()->hide();
	QTimer::singleShot(1000, this, SLOT(takeShot()));
}

void ScreenshotTaker::takeWindowShot()
{
	CurrentChatWidget->window()->hide();

	show();
}

void ScreenshotTaker::mouseReleaseEvent(QMouseEvent *e)
{
	Q_UNUSED(e)

	QPixmap pixmap = PixmapGrabber::grabCurrent();
	emit screenshotTaken(pixmap);

	CurrentChatWidget->window()->show();
}

void ScreenshotTaker::takeShot()
{
	QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	emit screenshotTaken(pixmap);

	CurrentChatWidget->window()->show();
}
