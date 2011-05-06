/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

#include "mpris-player-plugin.h"

MPRISPlayerPlugin::~MPRISPlayerPlugin()
{

}

int MPRISPlayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MPRISPlayerConfigurationUiHandler::registerConfigurationUi();
	MPRISPlayer::createInstance();

	return 0;
}

void MPRISPlayerPlugin::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
	MPRISPlayer::destroyInstance();
	MPRISPlayerConfigurationUiHandler::unregisterConfigurationUi();
}

Q_EXPORT_PLUGIN2(mprisplayer_mediaplayer, MPRISPlayerPlugin)
