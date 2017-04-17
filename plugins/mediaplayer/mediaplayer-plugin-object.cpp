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

#include "mediaplayer-plugin-object.h"
#include "mediaplayer-plugin-object.moc"

#include "mediaplayer-configuration-ui-handler.h"
#include "mediaplayer.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "misc/paths-provider.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"

MediaplayerPluginObject::MediaplayerPluginObject(QObject *parent) : QObject{parent}
{
}

MediaplayerPluginObject::~MediaplayerPluginObject()
{
}

void MediaplayerPluginObject::setConfigurationUiHandlerRepository(
    ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
    m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void MediaplayerPluginObject::setMainConfigurationWindowService(
    MainConfigurationWindowService *mainConfigurationWindowService)
{
    m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void MediaplayerPluginObject::setMediaplayerConfigurationUiHandler(
    MediaplayerConfigurationUiHandler *mediaplayerConfigurationUiHandler)
{
    m_mediaplayerConfigurationUiHandler = mediaplayerConfigurationUiHandler;
}

void MediaplayerPluginObject::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void MediaplayerPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

MediaPlayer *MediaplayerPluginObject::mediaPlayer() const
{
    return m_mediaPlayer;
}

void MediaplayerPluginObject::init()
{
    m_mainConfigurationWindowService->registerUiFile(
        m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/mediaplayer.ui"));
    m_configurationUiHandlerRepository->addConfigurationUiHandler(m_mediaplayerConfigurationUiHandler);
}

void MediaplayerPluginObject::done()
{
    m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_mediaplayerConfigurationUiHandler);
    m_mainConfigurationWindowService->unregisterUiFile(
        m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/mediaplayer.ui"));
}
