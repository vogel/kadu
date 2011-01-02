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
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QVBoxLayout>

#include "icons-manager.h"

#include "gui/widgets/chat-widget.h"
#include "pixmap-grabber.h"

#include "screenshot-taker.h"
#include <QKeyEvent>

ScreenshotTaker::ScreenshotTaker(ChatWidget *chatWidget, QWidget *parent) :
		QWidget(parent), CurrentChatWidget(chatWidget)
{
	setMouseTracking(true);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(tr("Drag icon to some window"), this));

	QLabel *crossLabel = new QLabel(this);
	crossLabel->setAlignment(Qt::AlignCenter);
	crossLabel->setPixmap(IconsManager::instance()->iconByPath("external_modules/screenshot-camera-photo").pixmap(16, 16));

	layout->addWidget(crossLabel, Qt::AlignCenter);
}

ScreenshotTaker::~ScreenshotTaker()
{
}

void ScreenshotTaker::takeStandardShot()
{
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

void ScreenshotTaker::closeEvent(QCloseEvent *e)
{
	emit screenshotNotTaken();

	CurrentChatWidget->window()->show();

	QWidget::closeEvent(e);
}

void ScreenshotTaker::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

void ScreenshotTaker::mousePressEvent(QMouseEvent *e)
{
	Q_UNUSED(e)

	setCursor(IconsManager::instance()->iconByPath("external_modules/screenshot-camera-photo").pixmap(32, 32));
}

void ScreenshotTaker::mouseReleaseEvent(QMouseEvent *e)
{
	Q_UNUSED(e)

	setCursor(Qt::ArrowCursor);

	QPixmap pixmap = PixmapGrabber::grabCurrent();

	hide();
	CurrentChatWidget->window()->show();

	emit screenshotTaken(pixmap, false);
}

void ScreenshotTaker::takeShot()
{
	QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());

	hide();
	CurrentChatWidget->window()->show();

	emit screenshotTaken(pixmap, true);
}
