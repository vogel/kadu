/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include "screenshot.h"
#include "screen-shot-saver.h"

ScreenShot::ScreenShot(ChatWidget *chatWidget) :
		MyChatWidget(chatWidget)
{
	kdebugf();

	MyScreenshotTaker = new ScreenshotTaker(MyChatWidget, this);
	connect(MyScreenshotTaker, SIGNAL(screenshotTaken(QPixmap)), this, SLOT(screenshotTaken(QPixmap)));

	// Rest stuff
	warnedAboutSize = false;
}

ScreenShot::~ScreenShot()
{
	kdebugf();
}

void ScreenShot::handleShot(QPixmap p)
{
	ScreenShotSaver *saver = new ScreenShotSaver(this);
	QString screenShotPath = saver->saveScreenShot(p);

	// TODO: 0.6.6
//	if (shotMode == WithChatWindowHidden || shotMode == SingleWindow)
	//	restore(chatWidget);

	// Wklejanie [IMAGE] do okna Chat
	if (ScreenShotConfiguration::instance()->pasteImageClauseIntoChatWidget())
	{
// 		// Sprawdzanie rozmiaru zrzutu wobec rozm�wc�w
		// TODO: 0.6.6
// 		UserListElements users = chatWidget->users()->toUserListElements();
// 		if (users.count() > 1)
// 			checkConferenceImageSizes(size);
// 		else
// 			if (!checkSingleUserImageSize(size))
// 				return;

		pasteImageClause(screenShotPath);
	}

	deleteLater();
}

void ScreenShot::shotNotCaptured()
{
	deleteLater();
}

void ScreenShot::pasteImageClause(const QString &path)
{
	MyChatWidget->edit()->insertPlainText(QString("[IMAGE ") + path + "]");
}

void ScreenShot::checkConferenceImageSizes(int size)
{
	Q_UNUSED(size)
	//TODO: 0.6.6, for now, assume it is ok
	/*
	UserListElements users = chatWidget->users()->toUserListElements();
	QStringList list;

	foreach (const UserListElement &user, users)
		if (user.protocolData("Gadu", "MaxImageSize").toInt() * 1024 < size)
			list.append(user.altNick());

	if (list.count() == 0)
		return;

	if (list.count() == users.count())
		MessageDialog::msg(tr("Image size is bigger than maximal image size\nset by all of conference contacts."), true);
	else
		MessageDialog::msg(tr("Image size is bigger than maximal image size\nset by some of conference contacts:\n%1.").arg(list.join(", ")), true);*/
}

bool ScreenShot::checkSingleUserImageSize(int size)
{
	Q_UNUSED(size)
	//TODO: 0.6.6, for now, assume it is ok
	return true;
	/*
	contacts = chatWidget->chat().contacts();

	if (users[0].protocolData("Gadu", "MaxImageSize").toInt() * 1024 >= size)
		return true;

	return MessageDialog::ask(tr("Image size is bigger than maximal image size set by %1. Send it anyway?").arg(users[0].altNick()));
	*/
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

void ScreenShot::screenshotTaken(QPixmap screenshot)
{
	ScreenshotWidget *screenshotWidget = new ScreenshotWidget(0);
	connect(screenshotWidget, SIGNAL(pixmapCaptured(QPixmap)), this, SLOT(handleShot(QPixmap)));
	connect(screenshotWidget, SIGNAL(closed()), this, SLOT(shotNotCaptured()));

	screenshotWidget->setPixmap(screenshot);
	screenshotWidget->setShotMode(Mode);
	screenshotWidget->showFullScreen();
	screenshotWidget->show();
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
	filters << prefix + "*";
	QFileInfoList list = dir.entryInfoList(filters, QDir::Files);

	foreach (const QFileInfo &f, list)
		size += f.size();

	if (size/1024 >= limit)
		ScreenshotNotification::notifySizeLimit(size);
}

void ScreenShot::takeWindowShot_Step2()
{
	kdebugf();
	QPixmap winPixmap = PixmapGrabber::grabCurrent();
	handleShot(winPixmap);
}
