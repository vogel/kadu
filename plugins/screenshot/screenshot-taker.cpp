/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/chat-widget/chat-widget.h"
#include "icons/kadu-icon.h"
#include "pixmap-grabber.h"

#include "screenshot-taker.h"

ScreenshotTaker::ScreenshotTaker(ChatWidget *chatWidget) :
		QWidget(chatWidget->window(), Qt::Window), CurrentChatWidget(chatWidget), Dragging(false)
{
	setWindowRole("kadu-screenshot-taker");

	setWindowModality(Qt::WindowModal);

	setAttribute(Qt::WA_DeleteOnClose);

	setMouseTracking(true);

	setWindowTitle(tr("Window Shot"));
	setWindowIcon(qApp->windowIcon()); // don't use status icon from the chat window!

	createLayout();

	connect(CancelButton, SIGNAL(clicked()), this, SLOT(close()));

	setFixedSize(sizeHint());
}

ScreenshotTaker::~ScreenshotTaker()
{
}

void ScreenshotTaker::createLayout()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	// label

	layout->addWidget(new QLabel(tr("Drag this icon onto the desired window"), this));

	// icon

	QHBoxLayout *iconLayout = new QHBoxLayout();
	iconLayout->addStretch();
	IconLabel = new QLabel(this);
	IconLabel->setAlignment(Qt::AlignCenter);
	IconLabel->setPixmap(KaduIcon("external_modules/screenshot-camera-photo").icon().pixmap(32, 32));
	iconLayout->addWidget(IconLabel);
	iconLayout->addStretch();

	layout->addLayout(iconLayout);

	// spacing

	layout->addSpacing(24);

	// cancel button

	QHBoxLayout *cancelLayout = new QHBoxLayout();
	cancelLayout->addStretch();
	CancelButton = new QPushButton(this);
	CancelButton->setText(tr("Cancel"));
	CancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	cancelLayout->addWidget(CancelButton);
	cancelLayout->addStretch();

	layout->addLayout(cancelLayout);
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
	show();
}

void ScreenshotTaker::closeEvent(QCloseEvent *e)
{
	emit screenshotNotTaken();

	CurrentChatWidget->window()->show();

	QWidget::closeEvent(e);
}

void ScreenshotTaker::mousePressEvent(QMouseEvent *e)
{
	if (childAt(e->pos()) != IconLabel)
		return;

	Dragging = true;

	setCursor(KaduIcon("external_modules/screenshot-camera-photo").icon().pixmap(32, 32));
}

void ScreenshotTaker::mouseReleaseEvent(QMouseEvent *e)
{
	Q_UNUSED(e)

	if (!Dragging)
		return;

	Dragging = false;

	setCursor(Qt::ArrowCursor);

	QPixmap pixmap = PixmapGrabber::grabCurrent();

	close();

	emit screenshotTaken(pixmap, false);
}

void ScreenshotTaker::takeShot()
{
	QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());

	hide();
	CurrentChatWidget->window()->show();

	emit screenshotTaken(pixmap, true);
}

#include "moc_screenshot-taker.cpp"
