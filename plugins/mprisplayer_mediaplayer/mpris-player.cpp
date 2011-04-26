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

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "mediaplayer/mediaplayer.h"
#include "misc/path-conversion.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"

#include "mpris-player.h"

const QString MPRISPlayer::PlayersListFile = "kadu/plugins/data/mprisplayer_mediaplayer/mprisplayer-players.data";
MPRISPlayer *MPRISPlayer::Instance = 0;

void MPRISPlayer::createInstance()
{
	if (!Instance)
		Instance = new MPRISPlayer();
}

void MPRISPlayer::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

MPRISPlayer::MPRISPlayer() :
	MPRISMediaPlayer(QString(), QString())
{
	replaceModule();
	configurationApplied();
}

MPRISPlayer::~MPRISPlayer()
{

}

void MPRISPlayer::replaceModule()
{
	QMap<QString , QString> replaceMap;
	replaceMap.insert("amarok2_mediaplayer", 	"Amarok");
	replaceMap.insert("audacious_mediaplayer", 	"Audacious");
	replaceMap.insert("bmpx_mediaplayer", 		"BMPx");
	replaceMap.insert("dragon_mediaplayer", 	"Dragon Player");
	replaceMap.insert("mpris_mediaplayer",		"MPRIS Media Player");
	replaceMap.insert("vlc_mediaplayer", 		"VLC");
	replaceMap.insert("xmms2_mediaplayer", 		"XMMS2");

	QMap<QString, Plugin *> loadedModules = PluginsManager::instance()->plugins();

	foreach (const QString &value, replaceMap)
	{
		QString key = replaceMap.key(value);
		if ((loadedModules.contains(key)) && (loadedModules.value(key)->state() == Plugin::PluginStateEnabled))
		{
			choosePlayer(key, value);
			loadedModules.value(key)->setState(Plugin::PluginStateDisabled);
			break;
		}
	}
}

void MPRISPlayer::choosePlayer(const QString &key, const QString &value)
{
	PlainConfigFile PlayersFile(dataPath(MPRISPlayer::playersListFileName()));

	// Save service value from mpris_mediaplayer module
	if (key == "mpris_mediaplayer")
	{
		QString oldMPRISService = config_file.readEntry("MediaPlayer", "MPRISService");

		PlayersFile.writeEntry(value, "player", value);
		PlayersFile.writeEntry(value, "service", oldMPRISService);
		PlayersFile.sync();

		config_file.writeEntry("MPRISPlayer", "Player", value);
		config_file.writeEntry("MPRISPlayer", "Service", oldMPRISService);
	}
	else // Choose player based on old module loaded.
	{
		config_file.writeEntry("MPRISPlayer", "Player", value);
		config_file.writeEntry("MPRISPlayer", "Service", PlayersFile.readEntry(value, "service"));
	}
}

void MPRISPlayer::configurationApplied()
{
	setName(config_file.readEntry("MPRISPlayer", "Player"));
	setService(config_file.readEntry("MPRISPlayer", "Service"));

	if (!MediaPlayer::instance()->registerMediaPlayer(MPRISPlayer::instance(), MPRISPlayer::instance()))
	{
		MediaPlayer::instance()->unregisterMediaPlayer();
		MediaPlayer::instance()->registerMediaPlayer(MPRISPlayer::instance(), MPRISPlayer::instance());
	}
}
