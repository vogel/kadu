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

#include "sound-plugin-object.h"

#include "gui/sound-actions.h"
#include "gui/sound-buddy-configuration-widget-factory.h"
#include "gui/sound-chat-configuration-widget-factory.h"
#include "gui/sound-configuration-ui-handler.h"
#include "notification/sound-notifier.h"
#include "sound-manager.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "notification/notification-manager.h"

SoundPluginObject::SoundPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

SoundPluginObject::~SoundPluginObject()
{
}

void SoundPluginObject::setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository)
{
	m_buddyConfigurationWidgetFactoryRepository = buddyConfigurationWidgetFactoryRepository;
}

void SoundPluginObject::setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository)
{
	m_chatConfigurationWidgetFactoryRepository = chatConfigurationWidgetFactoryRepository;
}

void SoundPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void SoundPluginObject::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void SoundPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SoundPluginObject::setSoundActions(SoundActions *soundActions)
{
	m_soundActions = soundActions;
}

void SoundPluginObject::setSoundBuddyConfigurationWidgetFactory(SoundBuddyConfigurationWidgetFactory *soundBuddyConfigurationWidgetFactory)
{
	m_soundBuddyConfigurationWidgetFactory = soundBuddyConfigurationWidgetFactory;
}

void SoundPluginObject::setSoundChatConfigurationWidgetFactory(SoundChatConfigurationWidgetFactory *soundChatConfigurationWidgetFactory)
{
	m_soundChatConfigurationWidgetFactory = soundChatConfigurationWidgetFactory;
}

void SoundPluginObject::setSoundConfigurationUiHandler(SoundConfigurationUiHandler *soundConfigurationUiHandler)
{
	m_soundConfigurationUiHandler = soundConfigurationUiHandler;
}

void SoundPluginObject::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

void SoundPluginObject::setSoundNotifier(SoundNotifier *soundNotifier)
{
	m_soundNotifier = soundNotifier;
}

SoundManager * SoundPluginObject::soundManager() const
{
	return m_soundManager;
}

void SoundPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_soundConfigurationUiHandler);
	m_buddyConfigurationWidgetFactoryRepository->registerFactory(m_soundBuddyConfigurationWidgetFactory);
	m_chatConfigurationWidgetFactoryRepository->registerFactory(m_soundChatConfigurationWidgetFactory);
	m_notificationManager->registerNotifier(m_soundNotifier);
}

void SoundPluginObject::done()
{
	m_notificationManager->unregisterNotifier(m_soundNotifier);
	m_chatConfigurationWidgetFactoryRepository->unregisterFactory(m_soundChatConfigurationWidgetFactory);
	m_buddyConfigurationWidgetFactoryRepository->unregisterFactory(m_soundBuddyConfigurationWidgetFactory);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_soundConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
}

#include "moc_sound-plugin-object.cpp"
