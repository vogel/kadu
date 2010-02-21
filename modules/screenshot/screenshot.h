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

#include "screenshot-mode.h"

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

	ScreenShotMode Mode;
	ScreenshotTaker *CurrentScreenshotTaker;

	QMenu *menu;
	bool wasMaximized;
	bool warnedAboutSize;

	void checkShotsSize();

	void pasteImageClause(const QString &path);
	void checkConferenceImageSizes(int size);
	bool checkSingleUserImageSize(int size);

	void createDefaultConfiguration();

private slots:
    void screenshotTaken(QPixmap screenshot);

	void grabScreenShot();

	void takeSimpleShot(ChatWidget *chatWidget);
	void takeShotWithChatWindowHidden(ChatWidget *chatWidget);
	void takeWindowShot(ChatWidget *chatWidget);

	void takeWindowShot_Step2();

	void handleShot(QPixmap pixmap);

public:
	ScreenShot(bool firstLoad);
	virtual ~ScreenShot();

};

extern ScreenShot* screenShot;

#endif // SCREENSHOT_H
