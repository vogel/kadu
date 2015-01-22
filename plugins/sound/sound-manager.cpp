/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002 Dariusz Jagodzik (mast3r@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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
#include <QtCore/QThread>
#include <QtMultimedia/QSound>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "debug.h"
#include "themes.h"

#include "sound-play-thread.h"
#include "sound-theme-manager.h"

#include "sound-manager.h"

SoundManager * SoundManager::Instance = 0;

void SoundManager::createInstance()
{
	if (!Instance)
		Instance = new SoundManager();
}

void SoundManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SoundManager::SoundManager() :
		Player{nullptr},
		CurrentSound{nullptr},
		Mute{false}
{
	kdebugf();

	createDefaultConfiguration();

	setMute(!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Sounds", "PlaySound"));

	PlayThread = new QThread();
	PlayThreadObject = new SoundPlayThread();
	PlayThreadObject->moveToThread(PlayThread);

	connect(PlayThread, SIGNAL(started()), PlayThreadObject, SLOT(start()));
	connect(PlayThreadObject, SIGNAL(finished()), PlayThread, SLOT(quit()), Qt::DirectConnection);
	connect(PlayThreadObject, SIGNAL(finished()), PlayThread, SLOT(deleteLater()), Qt::DirectConnection);

	PlayThread->start();

	kdebugf2();
}

SoundManager::~SoundManager()
{
	kdebugf();

	PlayThreadObject->end();

	PlayThread->wait(500);
	if (PlayThread->isRunning())
	{
		kdebugm(KDEBUG_WARNING, "terminating play_thread!\n");
		PlayThread->terminate();
		PlayThread->wait(200);
	}

	PlayThread->deleteLater();
	PlayThreadObject->deleteLater();

	delete CurrentSound;

	kdebugf2();
}

void SoundManager::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "ConnectionError_Sound", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "InvalidPassword_Sound", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "NewChat_Sound", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "NewMessage_Sound", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_Sound", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_Sound", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_Sound", true);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "PlaySound", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "SoundPaths", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "SoundTheme", "default");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "SoundVolume", 100);
}

bool SoundManager::isMuted() const
{
	return Mute;
}

void SoundManager::setMute(bool enable)
{
	Mute = enable;
}

void SoundManager::playFile(const QString &path, bool force)
{
	if (isMuted() && !force)
		return;

	if (!QFile::exists(path))
		return;

	if (CurrentSound && !CurrentSound->isFinished())
		return;

	if (Player)
	{
		PlayThreadObject->play(Player, path);
		return;
	}

	delete CurrentSound;
	CurrentSound = new QSound{path};
	CurrentSound->play();
}

void SoundManager::playSoundByName(const QString &soundName)
{
	if (isMuted())
		return;

	QString file = Application::instance()->configuration()->deprecatedApi()->readEntry("Sounds", soundName + "_sound");
	playFile(file);
}

void SoundManager::setPlayer(SoundPlayer *player)
{
	Player = player;
}

void SoundManager::testSoundPlaying()
{
	QString soundFile = SoundThemeManager::instance()->themes()->themePath() + SoundThemeManager::instance()->themes()->getThemeEntry("NewChat");
	playFile(soundFile, true);
}

#include "moc_sound-manager.cpp"
