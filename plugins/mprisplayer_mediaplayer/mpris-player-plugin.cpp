/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris-player-configuration-ui-handler.h"
#include "mpris-player.h"

#include "mpris-player-plugin.h"

MPRISPlayerPlugin::~MPRISPlayerPlugin()
{

}

bool MPRISPlayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MPRISPlayerConfigurationUiHandler::registerConfigurationUi();
	MPRISPlayer::createInstance();

	return true;
}

void MPRISPlayerPlugin::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
	MPRISPlayer::destroyInstance();
	MPRISPlayerConfigurationUiHandler::unregisterConfigurationUi();
}

Q_EXPORT_PLUGIN2(mprisplayer_mediaplayer, MPRISPlayerPlugin)

#include "moc_mpris-player-plugin.cpp"
