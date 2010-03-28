/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QMetaType>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/windows/configuration-window.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/path-list-edit.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "misc/misc.h"
#include "notify/notification.h"
#include "notify/notification-manager.h"
#include "parser/parser.h"
#include "debug.h"

#include "configuration/gui/sound-configuration-ui-handler.h"

#include "sample-play-thread.h"
#include "sound-exports.h"
#include "sound-file.h"
#include "sound-play-thread.h"
#include "sound-player.h"
#include "sound-slots.h"
#include "sound-theme-manager.h"

#include "sound.h"


/**
 * @ingroup sound
 * @{
 */
SOUNDAPI SoundManager *sound_manager = NULL;
SoundSlots *sound_slots;

SoundManager::SoundManager(bool firstLoad) :
		Notifier("Sound", "Play a sound", IconsManager::instance()->iconByPath("16x16/audio-volume-high.png")),
		Player(0),
		LastSoundTime(), Mute(false),
		PlayThread(new SoundPlayThread()), SimplePlayerCount(0)
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();

	LastSoundTime.start();

	PlayThread->start();

	sound_manager = this;
	sound_slots = new SoundSlots(firstLoad, this);

	SoundThemeManager::instance()->themes()->setPaths(config_file.readEntry("Sounds", "SoundPaths").split(QRegExp("(;|:)"), QString::SkipEmptyParts));

	QStringList soundThemes = SoundThemeManager::instance()->themes()->themes();
	QString soundTheme = config_file.readEntry("Sounds", "SoundTheme");
	if (!soundThemes.isEmpty() && (soundTheme != "Custom") && !soundThemes.contains(soundTheme))
	{
		soundTheme = "default";
		config_file.writeEntry("Sounds", "SoundTheme", "default");
	}

	if (soundTheme != "custom")
		SoundThemeManager::instance()->applyTheme(soundTheme);

	NotificationManager::instance()->registerNotifier(this);

	kdebugf2();
}

SoundManager::~SoundManager()
{
	kdebugf();
	PlayThread->end();
	NotificationManager::instance()->unregisterNotifier(this);

	PlayThread->wait(2000);
	if (PlayThread->isRunning())
	{
		kdebugm(KDEBUG_WARNING, "terminating play_thread!\n");
		PlayThread->terminate();
	}
	delete PlayThread;
	delete sound_slots;
	sound_slots = 0;

	kdebugf2();
}

NotifierConfigurationWidget * SoundManager::createConfigurationWidget(QWidget *parent)
{
	return SoundConfigurationUiHandler::instance()->createConfigurationWidget(parent);
}

void SoundManager::import_0_6_5_configuration()
{
	config_file.addVariable("Notify", "StatusChanged/ToAway_Sound",
			config_file.readEntry("Notify", "StatusChanged/ToAway_Sound"));
}

void SoundManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "ConnectionError_Sound", true);
	config_file.addVariable("Notify", "NewChat_Sound", true);
	config_file.addVariable("Notify", "NewMessage_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToAway_Sound", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Sound", true);

	config_file.addVariable("Sounds", "PlaySound", true);
	config_file.addVariable("Sounds", "SoundPaths", "");
	config_file.addVariable("Sounds", "SoundTheme", "default");
	config_file.addVariable("Sounds", "SoundVolume", 100);
	config_file.addVariable("Sounds", "VolumeControl", false);
}

bool SoundManager::isMuted() const
{
	return Mute;
}

void SoundManager::setMute(const bool &enable)
{
	Mute = enable;
}

void SoundManager::playSound(const QString &soundName)
{
	if (isMuted())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: muted\n");
		return;
	}

	if (timeAfterLastSound() < 500)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: too often, exiting\n");
		return;
	}

	QString sound = config_file.readEntry("Sounds", soundName + "_sound");

	if (QFile::exists(sound))
	{
		play(sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0 * config_file.readDoubleNumEntry("Sounds", "SoundVolume") / 100);
		LastSoundTime.restart();
	}
	else
		fprintf(stderr, "file (%s) not found\n", qPrintable(sound));
}

void SoundManager::notify(Notification *notification)
{
	kdebugf();

	playSound(notification->key());

	kdebugf2();
}

void SoundManager::setPlayer(SoundPlayer *player)
{
	Player = player;
}

void SoundManager::play(const QString &path, bool force)
{
	kdebugf();

	if (isMuted() && !force)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: muted\n");
		return;
	}

	if (QFile::exists(path))
		play(path, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0 * config_file.readDoubleNumEntry("Sounds", "SoundVolume") / 100);
	else
		fprintf(stderr, "file (%s) not found\n", qPrintable(path));

	kdebugf2();
}

int SoundManager::timeAfterLastSound() const
{
	return LastSoundTime.elapsed();
}

// stupid Qt, yes this code work
void SoundManager::connectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>> %s %s\n", signal, SIGNAL(playSound(QString&,bool,double)) );
	if (strcmp(signal, SIGNAL(playSound(QString, bool, double))) == 0)
		++SimplePlayerCount;
}

void SoundManager::disconnectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>> %s %s\n", signal, SIGNAL(playSound(QString&,bool,double)) );
	if (strcmp(signal, SIGNAL(playSound(QString, bool, double))) == 0)
		--SimplePlayerCount;
}

void SoundManager::play(const QString &path, bool volumeControl, double volume)
{
	kdebugf();

	if (!Player)
		return;

	if (Player->isSimplePlayer())
		PlayThread->play(Player, path, volumeControl, volume);

	kdebugf2();
}

/** @} */

