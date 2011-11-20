/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtGui/QApplication>

#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/message-dialog.h"
#include "debug.h"

#include "configuration/screen-shot-configuration.h"
#include "gui/widgets/screenshot-widget.h"
#include "notify/screenshot-notification.h"
#include "pixmap-grabber.h"
#include "screenshot-taker.h"

#include "screen-shot-saver.h"
#include "screenshot.h"

ScreenShot::ScreenShot(ChatWidget *chatWidget) :
		MyChatWidget(chatWidget)
{
	kdebugf();

	MyScreenshotTaker = new ScreenshotTaker(MyChatWidget);
	connect(MyScreenshotTaker, SIGNAL(screenshotTaken(QPixmap, bool)), this, SLOT(screenshotTaken(QPixmap, bool)));
	connect(MyScreenshotTaker, SIGNAL(screenshotNotTaken()), this, SLOT(screenshotNotTaken()));

	// Rest stuff
	warnedAboutSize = false;
}

ScreenShot::~ScreenShot()
{
	delete MyScreenshotTaker;
	MyScreenshotTaker = 0;
}

void ScreenShot::takeStandardShot()
{
	MyScreenshotTaker->takeStandardShot();
}

void ScreenShot::takeShotWithChatWindowHidden()
{
	MyScreenshotTaker->takeShotWithChatWindowHidden();
}

void ScreenShot::takeWindowShot()
{
	MyScreenshotTaker->takeWindowShot();
}

void ScreenShot::screenshotTaken(QPixmap screenshot, bool needsCrop)
{
	if (!needsCrop)
	{
		screenshotReady(screenshot);
		return;
	}

	ScreenshotWidget *screenshotWidget = new ScreenshotWidget(0);
	connect(screenshotWidget, SIGNAL(pixmapCaptured(QPixmap)), this, SLOT(screenshotReady(QPixmap)));
	connect(screenshotWidget, SIGNAL(closed()), this, SLOT(screenshotNotTaken()));

	screenshotWidget->setPixmap(screenshot);
	screenshotWidget->setShotMode(Mode);
	screenshotWidget->showFullScreen();
	screenshotWidget->show();
}

void ScreenShot::screenshotNotTaken()
{
	deleteLater();
}

void ScreenShot::screenshotReady(QPixmap p)
{
	ScreenShotSaver *saver = new ScreenShotSaver(this);
	QString screenShotPath = saver->saveScreenShot(p);

	if (ScreenShotConfiguration::instance()->pasteImageClauseIntoChatWidget())
	{
		pasteImageClause(screenShotPath);
		if (!checkImageSize(saver->size()))
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Image size is bigger than maximal image size for this chat."));
	}

	deleteLater();
}

void ScreenShot::pasteImageClause(const QString &path)
{
	MyChatWidget->edit()->insertPlainText(QString("[IMAGE ") + path + ']');
}

bool ScreenShot::checkImageSize(long int size)
{
	Q_UNUSED(size)

	ContactSet contacts = MyChatWidget->chat().contacts();
	foreach (const Contact &contact, contacts)
		if (contact.maximumImageSize() * 1024 < size)
			return false;

	return true;
}

void ScreenShot::checkShotsSize()
{
	kdebugf();
	if (!ScreenShotConfiguration::instance()->warnAboutDirectorySize())
		return;

	int size = 0;

	int limit = ScreenShotConfiguration::instance()->directorySizeLimit();
	QDir dir(ScreenShotConfiguration::instance()->imagePath());

	QString prefix = ScreenShotConfiguration::instance()->fileNamePrefix();
	QStringList filters;
	filters << prefix + '*';
	QFileInfoList list = dir.entryInfoList(filters, QDir::Files);

	foreach (const QFileInfo &f, list)
		size += f.size();

	if (size/1024 >= limit)
		ScreenshotNotification::notifySizeLimit(size);
}
