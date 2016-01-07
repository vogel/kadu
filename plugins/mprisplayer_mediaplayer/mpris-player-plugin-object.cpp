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

#include "mpris-player-plugin-object.h"

#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

#include "plugins/mediaplayer/mediaplayer-plugin-object.h"
#include "plugins/mediaplayer/mediaplayer.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-repository.h"

MprisPlayerPluginObject::MprisPlayerPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

MprisPlayerPluginObject::~MprisPlayerPluginObject()
{
}

void MprisPlayerPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void MprisPlayerPluginObject::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	m_mediaPlayer = mediaPlayer;
}

void MprisPlayerPluginObject::setMPRISPlayerConfigurationUiHandler(MPRISPlayerConfigurationUiHandler *mprisPlayerConfigurationUiHandler)
{
	m_mprisPlayerConfigurationUiHandler = mprisPlayerConfigurationUiHandler;
}

void MprisPlayerPluginObject::setMPRISPlayer(MPRISPlayer *mprisPlayer)
{
	m_mprisPlayer = mprisPlayer;
}

void MprisPlayerPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void MprisPlayerPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_mprisPlayerConfigurationUiHandler);

	if (!m_mediaPlayer->registerMediaPlayer(m_mprisPlayer, m_mprisPlayer))
	{
		m_mediaPlayer->unregisterMediaPlayer();
		m_mediaPlayer->registerMediaPlayer(m_mprisPlayer, m_mprisPlayer);
	}

	m_mprisPlayer->configurationApplied();
}

void MprisPlayerPluginObject::done()
{
	m_mediaPlayer->unregisterMediaPlayer();
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_mprisPlayerConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/mprisplayer_mediaplayer.ui"));
}

#include "moc_mpris-player-plugin-object.cpp"
