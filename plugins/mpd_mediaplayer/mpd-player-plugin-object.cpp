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

#include "mpd-player-plugin-object.h"

#include "mpd-mediaplayer.h"

#include "plugins/mediaplayer/mediaplayer-plugin-object.h"
#include "plugins/mediaplayer/mediaplayer.h"

#include "windows/main-configuration-window-service.h"
#include "misc/paths-provider.h"

MpdPlayerPluginObject::MpdPlayerPluginObject(QObject *parent) :
		QObject{parent}
{
}

MpdPlayerPluginObject::~MpdPlayerPluginObject()
{
}

void MpdPlayerPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void MpdPlayerPluginObject::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	m_mediaPlayer = mediaPlayer;
}

void MpdPlayerPluginObject::setMPDMediaPlayer(MPDMediaPlayer *mpdMediaPlayer)
{
	m_mpdMediaPlayer = mpdMediaPlayer;
}

void MpdPlayerPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void MpdPlayerPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/mpd_config.ui"));
	m_mediaPlayer->registerMediaPlayer(m_mpdMediaPlayer, m_mpdMediaPlayer);
}

void MpdPlayerPluginObject::done()
{
	m_mediaPlayer->unregisterMediaPlayer();
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/mpd_config.ui"));
}

#include "moc_mpd-player-plugin-object.cpp"
