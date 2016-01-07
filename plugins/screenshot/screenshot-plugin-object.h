/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class NotificationEventRepository;
class PathsProvider;
class ScreenshotActions;
class ScreenShotConfigurationUiHandler;
class ScreenShotConfiguration;

class ScreenshotPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit ScreenshotPluginObject(QObject *parent = nullptr);
	virtual ~ScreenshotPluginObject();

private:
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<ScreenshotActions> m_screenshotActions;
	QPointer<ScreenShotConfigurationUiHandler> m_screenShotConfigurationUiHandler;
	QPointer<ScreenShotConfiguration> m_screenShotConfiguration;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SET void setScreenshotActions(ScreenshotActions *screenshotActions);
	INJEQT_SET void setScreenShotConfigurationUiHandler(ScreenShotConfigurationUiHandler *screenShotConfigurationUiHandler);
	INJEQT_SET void setScreenShotConfiguration(ScreenShotConfiguration *screenShotConfiguration);

};
