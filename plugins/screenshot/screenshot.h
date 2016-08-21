/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QRect>
#include <QtGui/QPixmap>
#include <injeqt/injeqt.h>

#include "screenshot-mode.h"

class QAction;
class QLabel;
class QMenu;

class ActionDescription;
class ChatWidget;
class ConfigurationUiHandler;
class Configuration;
class CropImageWidget;
class IconsManager;
class PluginInjectedFactory;
class ScreenShotConfiguration;
class ScreenshotNotificationService;
class ScreenshotTaker;
class ScreenshotToolBox;
class ScreenshotWidget;

class ScreenShot : public QObject
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<ScreenShotConfiguration> m_screenShotConfiguration;
	QPointer<ScreenshotNotificationService> m_screenshotNotificationService;

	ScreenShotMode Mode;
	ScreenshotTaker *MyScreenshotTaker;
	ChatWidget *MyChatWidget;

	bool warnedAboutSize;

	void checkShotsSize();

	void pasteImageClause(const QString &path);
	bool checkImageSize(long int size);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setScreenshotNotificationService(ScreenshotNotificationService *screenshotNotificationService);
	INJEQT_INIT void init();

	void screenshotTaken(QPixmap screenshot, bool needsCrop);
	void screenshotNotTaken();

	void screenshotReady(QPixmap pixmap);

public:
	explicit ScreenShot(ScreenShotConfiguration *screenShotConfiguration, ChatWidget *chatWidget);
	virtual ~ScreenShot();

	void takeStandardShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();

};
