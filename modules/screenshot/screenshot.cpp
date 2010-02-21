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

#include <algorithm>
#include <vector>

#include <QtCore/QBuffer>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QRect>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QImageWriter>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QX11Info>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "notify/notification.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "debug.h"

#include "gui/widgets/crop-image-widget.h"
#include "gui/widgets/screenshot-tool-box.h"
#include "gui/widgets/screenshot-widget.h"
#include "pixmap-grabber.h"
#include "screenshot-configuration-ui-handler.h"
#include "screenshot-taker.h"

#include "screenshot.h"

ScreenShot *screenShot;
NotifyEvent *ScreenShotImageSizeLimit = 0;

extern "C" int screenshot_init(bool firstLoad)
{
	kdebugf();

	ScreenShotImageSizeLimit = new NotifyEvent("ssSizeLimit", NotifyEvent::CallbackNotRequired, "ScreenShot images size limit");

	screenShot = new ScreenShot(firstLoad);
	NotificationManager::instance()->registerNotifyEvent(ScreenShotImageSizeLimit);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));
	MainConfigurationWindow::registerUiHandler(screenShot->configurationUiHandler());

	return 0;
}

extern "C" void screenshot_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));
	MainConfigurationWindow::unregisterUiHandler(screenShot->configurationUiHandler());
	NotificationManager::instance()->unregisterNotifyEvent(ScreenShotImageSizeLimit);

	delete ScreenShotImageSizeLimit;
	ScreenShotImageSizeLimit = 0;

	delete screenShot;
	screenShot = 0;
}

ScreenShot::ScreenShot(bool firstLoad) :
		CurrentScreenshotWidget(0)
{
	kdebugf();

	CurrentScreenshotTaker = new ScreenshotTaker(this);
	connect(CurrentScreenshotTaker, SIGNAL(screenshotTaken(QPixmap)), this, SLOT(screenshotTaken(QPixmap)));

	CurrentScreenshotWidget = new ScreenshotWidget(0);
	connect(CurrentScreenshotWidget, SIGNAL(pixmapCaptured(QPixmap)), this, SLOT(pixmapCropped(QPixmap)));

	createMenu();

	UiHandler = new ScreenShotConfigurationUiHandler();

	// Chat toolbar button
	screenShotAction = new ActionDescription(this,
		ActionDescription::TypeChat, "ScreenShotAction",
		this, SLOT(screenshotActionActivated(QAction *, bool)),
		"external_modules/module_screenshot_shot-camera-photo.png", "external_modules/module_screenshot_shot-camera-photo.png", tr("ScreenShot"), false, ""
	);

	if (firstLoad)
		ChatEditBox::addAction("ScreenShotAction");

	// Rest stuff
	warnedAboutSize = false;

	createDefaultConfiguration();
}

ScreenShot::~ScreenShot()
{
	kdebugf();

	// TODO: 0.6.6
// 	delete UiHandler;
	delete screenShotAction;

	delete menu;
	delete CurrentScreenshotWidget;
}

void ScreenShot::createMenu()
{
	// Chat windows menu
	menu = new QMenu();
	menu->addAction(tr("Simple shot"), CurrentScreenshotTaker, SLOT(takeStandardShot()));
	menu->addAction(tr("With chat window hidden"), CurrentScreenshotTaker, SLOT(takeShotWithChatWindowHidden()));
	menu->addAction(tr("Window shot"), CurrentScreenshotTaker, SLOT(takeWindowShot()));
}

void ScreenShot::screenshotActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		QWidget *widget = widgets[widgets.size() - 1];
		menu->popup(widget->mapToGlobal(QPoint(0, widget->height())));
	}
}

void ScreenShot::handleShot(QPixmap p)
{
	if (!chatWidget)
		return;

	// Plik do zapisu:
	QString dirPath = config_file.readEntry("ScreenShot", "path", profilePath("images/"));

	QDir dir(dirPath);
	if (!dir.exists() && !dir.mkpath(dirPath))
	{
		MessageDialog::msg(tr("Unable to create direcotry %1 for storing screenshots!").arg(dirPath));
		return;
	}

	bool useShortJpg = config_file.readBoolEntry("ScreenShot", "use_short_jpg", false);

	QString ext = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toLower();
	if (useShortJpg && ext == "jpeg")
		ext = "jpg";

	QString path = QDir::cleanPath(
		dir.absolutePath() + "/" +
		config_file.readEntry("ScreenShot", "filenamePrefix", "shot") +
		QString::number(QDateTime::currentDateTime().toTime_t()) + "." +
		ext
	);

	const char *format = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toAscii();
	int quality = config_file.readNumEntry("ScreenShot", "quality", -1);
	if (!p.save(path, format, quality))
	{
		MessageDialog::msg(tr("Can't write file %1.\nAccess denied or other problem!").arg(path));
		return;
	}

	QFileInfo f(path);
	int size = f.size();

	if (size == 0)
	{
		MessageDialog::msg(tr("Screenshot %1 has 0 size!\nIt should be bigger.").arg(path));
		return;
	}

	// TODO: 0.6.6
//	if (shotMode == WithChatWindowHidden || shotMode == SingleWindow)
	//	restore(chatWidget);

	// Wklejanie [IMAGE] do okna Chat
	if (config_file.readBoolEntry("ScreenShot", "paste_clause", true))
	{
		// Sprawdzanie rozmiaru zrzutu wobec rozm�wc�w
		// TODO: 0.6.6
// 		UserListElements users = chatWidget->users()->toUserListElements();
// 		if (users.count() > 1)
// 			checkConferenceImageSizes(size);
// 		else
// 			if (!checkSingleUserImageSize(size))
// 				return;

		pasteImageClause(path);
	}

	chatWidget = 0;
	checkShotsSize();
}

void ScreenShot::pasteImageClause(const QString &path)
{
	chatWidget->edit()->insertPlainText(QString("[IMAGE ") + path + "]");
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

void ScreenShot::takeSimpleShot()
{
	kdebugf();

	CurrentScreenshotWidget->setShotMode(ShotModeStandard);

	QTimer::singleShot(1000, this, SLOT(takeShot_Step2()));
	chatWidget->update();
	qApp->processEvents();
}

void ScreenShot::takeShotWithChatWindowHidden()
{
	CurrentScreenshotWidget->setShotMode(ShotModeWithChatWindowHidden);

// 	wasMaximized = isMaximized(chatWidget);
// 	minimize(chatWidget);
// 	QTimer::singleShot(600, this, SLOT(takeShot_Step2()));
}

void ScreenShot::takeWindowShot()
{
	CurrentScreenshotWidget->setShotMode(ShotModeSingleWindow);

// 	wasMaximized = isMaximized(chatWidget);
// 	minimize(chatWidget);

// 	takeShot_Step2();
}

void ScreenShot::screenshotTaken(QPixmap screenshot)
{
	CurrentScreenshotWidget->setPixmap(screenshot);

	CurrentScreenshotWidget->showFullScreen();
	CurrentScreenshotWidget->show();

	QTimer::singleShot(100, this, SLOT(grabMouseSlot()));
}

void ScreenShot::pixmapCropped(QPixmap pixmap)
{
	handleShot(pixmap);
	CurrentScreenshotWidget->hide();
}

void ScreenShot::grabMouseSlot()
{
	kdebugf();

// 	CurrentScreenshotWidget->grabMouse();
// 	CurrentScreenshotWidget->grabKeyboard();
}

void ScreenShot::checkShotsSize()
{
	kdebugf();
	if (!config_file.readBoolEntry("ScreenShot", "dir_size_warns", true))
		return;

	int size = 0;

	int limit = config_file.readNumEntry("ScreenShot", "dir_size_limit", 10000);
	QDir dir(config_file.readEntry("ScreenShot", "path", profilePath("images")));

	QString prefix = config_file.readEntry("ScreenShot", "filenamePrefix", "shot");
	QStringList filters;
	filters << prefix + "*";
	QFileInfoList list = dir.entryInfoList(filters, QDir::Files);

	foreach (const QFileInfo &f, list)
		size += f.size();

	if (size/1024 >= limit)
	{
		Notification *notification = new Notification("ssSizeLimit", IconsManager::instance()->iconByPath("kadu_icons/kadu-blocking.png"));
		notification->setTitle(tr("ScreenShot size limit"));
		notification->setText(tr("Images size limit exceed: %1 KB").arg(size/1024));
		NotificationManager::instance()->notify(notification);
	}
}

void ScreenShot::takeWindowShot_Step2()
{
	kdebugf();
	QPixmap winPixmap = PixmapGrabber::grabCurrent();
	handleShot(winPixmap);
}

void ScreenShot::createDefaultConfiguration()
{
	config_file.addVariable("ScreenShot", "fileFormat", "PNG");
	config_file.addVariable("ScreenShot", "use_short_jpg", true);
	config_file.addVariable("ScreenShot", "quality", -1);
	config_file.addVariable("ScreenShot", "path", profilePath("images/"));
	config_file.addVariable("ScreenShot", "filenamePrefix", "shot");
	config_file.addVariable("ScreenShot", "paste_clause", true);
	config_file.addVariable("ScreenShot", "dir_size_warns", true);
	config_file.addVariable("ScreenShot", "dir_size_limit", 10000);
}
