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

#include "sound-exports.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyConfigurationWidgetFactoryRepository;
class ChatConfigurationWidgetFactoryRepository;
class ConfigurationUiHandlerRepository;
class MainConfigurationWindowService;
class NotificationManager;
class PathsProvider;
class SoundActions;
class SoundBuddyConfigurationWidgetFactory;
class SoundChatConfigurationWidgetFactory;
class SoundConfigurationUiHandler;
class SoundManager;
class SoundNotifier;

class SOUNDAPI SoundPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit SoundPluginObject(QObject *parent = nullptr);
	virtual ~SoundPluginObject();

	SoundManager * soundManager() const;

private:
	QPointer<BuddyConfigurationWidgetFactoryRepository> m_buddyConfigurationWidgetFactoryRepository;
	QPointer<ChatConfigurationWidgetFactoryRepository> m_chatConfigurationWidgetFactoryRepository;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MainConfigurationWindowService> m_mainConfigurationWindowService;
	QPointer<NotificationManager> m_notificationManager;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<SoundActions> m_soundActions;
	QPointer<SoundBuddyConfigurationWidgetFactory> m_soundBuddyConfigurationWidgetFactory;
	QPointer<SoundChatConfigurationWidgetFactory> m_soundChatConfigurationWidgetFactory;
	QPointer<SoundConfigurationUiHandler> m_soundConfigurationUiHandler;
	QPointer<SoundManager> m_soundManager;
	QPointer<SoundNotifier> m_soundNotifier;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository);
	INJEQT_SET void setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository);
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService);
	INJEQT_SET void setNotificationManager(NotificationManager *notificationManager);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SET void setSoundActions(SoundActions *soundActions);
	INJEQT_SET void setSoundBuddyConfigurationWidgetFactory(SoundBuddyConfigurationWidgetFactory *soundBuddyConfigurationWidgetFactory);
	INJEQT_SET void setSoundChatConfigurationWidgetFactory(SoundChatConfigurationWidgetFactory *soundChatConfigurationWidgetFactory);
	INJEQT_SET void setSoundConfigurationUiHandler(SoundConfigurationUiHandler *soundConfigurationUiHandler);
	INJEQT_SET void setSoundManager(SoundManager *soundManager);
	INJEQT_SET void setSoundNotifier(SoundNotifier *soundNotifier);

};
