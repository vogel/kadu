/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QSettings>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/main-configuration-window.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state.h"

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris-player.h"

const QString MPRISPlayer::UserPlayersListFile = "mprisplayer-players.data";
const QString MPRISPlayer::GlobalPlayersListFile = "plugins/data/mprisplayer_mediaplayer/" + MPRISPlayer::UserPlayersListFile;
MPRISPlayer *MPRISPlayer::Instance = 0;

void MPRISPlayer::createInstance()
{
	if (!Instance)
		Instance = new MPRISPlayer();

	Instance->configurationApplied();
}

void MPRISPlayer::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

MPRISPlayer::MPRISPlayer(QObject *parent) :
		MPRISMediaPlayer(parent)
{
	prepareUserPlayersFile();
	replacePlugin();
}

MPRISPlayer::~MPRISPlayer()
{

}

void MPRISPlayer::prepareUserPlayersFile()
{
	if (QFile::exists(MPRISPlayer::userPlayersListFileName()))
		return;

	QFile userFile(MPRISPlayer::userPlayersListFileName());
	if (!userFile.open(QIODevice::ReadWrite))
		return;

	userFile.close();
}

void MPRISPlayer::replacePlugin()
{
	QMap<QString, QString> replaceMap;
	replaceMap.insert("amarok2_mediaplayer", 	"Amarok");
	replaceMap.insert("audacious_mediaplayer", 	"Audacious");
	replaceMap.insert("bmpx_mediaplayer", 		"BMPx");
	replaceMap.insert("dragon_mediaplayer", 	"Dragon Player");
	replaceMap.insert("mpris_mediaplayer",		"MPRIS Media Player");
	replaceMap.insert("vlc_mediaplayer", 		"VLC");
	replaceMap.insert("xmms2_mediaplayer", 		"XMMS2");

	foreach (const QString &value, replaceMap)
	{
		QString key = replaceMap.key(value);
		if (Core::instance()->pluginStateService()->pluginState(key) == PluginState::Enabled)
		{
			choosePlayer(key, value);
			Core::instance()->pluginStateService()->setPluginState(key, PluginState::Disabled);
			break;
		}
	}
}

void MPRISPlayer::choosePlayer(const QString &key, const QString &value)
{
	// Save service value from mpris_mediaplayer module
	if (key == "mpris_mediaplayer")
	{
		QString oldMPRISService = Application::instance()->configuration()->deprecatedApi()->readEntry("MediaPlayer", "MPRISService");
		QSettings userPlayersSettings(MPRISPlayer::userPlayersListFileName(), QSettings::IniFormat);
		userPlayersSettings.setIniCodec("ISO8859-2");

		userPlayersSettings.setValue(value + "/player", value);
		userPlayersSettings.setValue(value +  "/service", oldMPRISService);
		userPlayersSettings.sync();

		Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Player", value);
		Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Service", oldMPRISService);
	}
	else // Choose player based on old module loaded.
	{
		QSettings globalPlayersSettings(MPRISPlayer::globalPlayersListFileName(), QSettings::IniFormat);
		globalPlayersSettings.setIniCodec("ISO8859-2");

		Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Player", value);
		Application::instance()->configuration()->deprecatedApi()->writeEntry("MPRISPlayer", "Service", globalPlayersSettings.value(value + "/service").toString());
	}
}

void MPRISPlayer::configurationApplied()
{
	setName(Application::instance()->configuration()->deprecatedApi()->readEntry("MPRISPlayer", "Player"));
	setService(Application::instance()->configuration()->deprecatedApi()->readEntry("MPRISPlayer", "Service"));

	if (!MediaPlayer::instance()->registerMediaPlayer(MPRISPlayer::instance(), MPRISPlayer::instance()))
	{
		MediaPlayer::instance()->unregisterMediaPlayer();
		MediaPlayer::instance()->registerMediaPlayer(MPRISPlayer::instance(), MPRISPlayer::instance());
	}
}

#include "moc_mpris-player.cpp"
