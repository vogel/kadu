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

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtGui/QPixmap>

class QAction;
class QLabel;
class QMenu;

class ActionDescription;
class ChatWidget;
class ConfigurationUiHandler;
class CropImageWidget;
class ScreenshotTaker;
class ScreenshotToolBox;
class ScreenshotWidget;

class ScreenShot : public QObject
{
	Q_OBJECT

	ActionDescription *screenShotAction;
	ChatWidget *chatWidget;

	ScreenshotTaker *CurrentScreenshotTaker;
	ScreenshotWidget *CurrentScreenshotWidget;

	QMenu *menu;
	bool wasMaximized;
	bool warnedAboutSize;

	void createMenu();

	void checkShotsSize();

	void pasteImageClause(const QString &path);
	void checkConferenceImageSizes(int size);
	bool checkSingleUserImageSize(int size);

	void createDefaultConfiguration();

private slots:
    void screenshotTaken(QPixmap screenshot);
	void pixmapCropped(QPixmap pixmap);

	void takeSimpleShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();

	void takeWindowShot_Step2();

	void grabMouseSlot();

	void screenshotActionActivated(QAction *sender, bool toggled);

	void handleShot(QPixmap pixmap);

public:
	ScreenShot(bool firstLoad);
	virtual ~ScreenShot();

};

extern ScreenShot* screenShot;

#endif // SCREENSHOT_H
