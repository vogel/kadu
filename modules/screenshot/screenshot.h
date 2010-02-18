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

#include <X11/extensions/shape.h>

class QAction;
class QLabel;
class QMenu;

class ActionDescription;
class ChatWidget;
class ConfigurationUiHandler;
class CropImageWidget;
class ScreenshotToolBox;
class ScreenshotWidget;

class ScreenShot : public QObject
{
	Q_OBJECT

	ConfigurationUiHandler *UiHandler;
	ActionDescription *screenShotAction;
	ChatWidget *chatWidget;

	ScreenshotWidget *CurrentScreenshotWidget;


	QMenu *menu;
	bool wasMaximized;
	bool warnedAboutSize;
	int minSize;

	void checkShotsSize();

	void pasteImageClause(const QString &path);
	void checkConferenceImageSizes(int size);
	bool checkSingleUserImageSize(int size);

	// TABS module fixing methods
	void minimize(QWidget *w);
	void restore(QWidget *w);
	bool isMaximized(QWidget *w);

	// Methods from KSnapShot
	QPixmap grabCurrent();
	Window findRealWindow(Window w, int depth = 0);
	void getWindowsRecursive(std::vector<QRect> &windows, Window w, int rx = 0, int ry = 0, int depth = 0);
	QPixmap grabWindow(Window child, int x, int y, uint w, uint h, uint border);
	Window windowUnderCursor(bool includeDecorations);

	void createDefaultConfiguration();

private slots:
	void takeShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();


	void takeShot_Step2();
	void takeWindowShot_Step2();

	void grabMouseSlot();

	void screenshotActionActivated(QAction *sender, bool toggled);

	void handleShot(QPixmap pixmap);

public:
	ScreenShot(bool firstLoad);
	virtual ~ScreenShot();

	ConfigurationUiHandler * configurationUiHandler() { return UiHandler; }

};

extern ScreenShot* screenShot;

#endif
