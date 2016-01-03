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

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyConfigurationWidgetFactoryRepository;
class ChatConfigurationWidgetFactoryRepository;
class ConfigurationUiHandlerRepository;
class NotificationManager;
class PathsProvider;
class SoundActions;
class SoundBuddyConfigurationWidgetFactory;
class SoundChatConfigurationWidgetFactory;
class SoundConfigurationUiHandler;
class SoundManager;
class SoundNotifier;

class SOUNDAPI SoundPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SoundPluginObject(QObject *parent = nullptr);
	virtual ~SoundPluginObject();

	SoundManager * soundManager() const;

private:
	QPointer<BuddyConfigurationWidgetFactoryRepository> m_buddyConfigurationWidgetFactoryRepository;
	QPointer<ChatConfigurationWidgetFactoryRepository> m_chatConfigurationWidgetFactoryRepository;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
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
	INJEQT_SETTER void setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setNotificationManager(NotificationManager *notificationManager);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SETTER void setSoundActions(SoundActions *soundActions);
	INJEQT_SETTER void setSoundBuddyConfigurationWidgetFactory(SoundBuddyConfigurationWidgetFactory *soundBuddyConfigurationWidgetFactory);
	INJEQT_SETTER void setSoundChatConfigurationWidgetFactory(SoundChatConfigurationWidgetFactory *soundChatConfigurationWidgetFactory);
	INJEQT_SETTER void setSoundConfigurationUiHandler(SoundConfigurationUiHandler *soundConfigurationUiHandler);
	INJEQT_SETTER void setSoundManager(SoundManager *soundManager);
	INJEQT_SETTER void setSoundNotifier(SoundNotifier *soundNotifier);

};
