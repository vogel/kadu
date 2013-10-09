/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QProcess>

#include "configuration/configuration-file.h"
#include "debug.h"

#include "external-player.h"

ExternalPlayer * ExternalPlayer::Instance = 0;

void ExternalPlayer::createInstance()
{
	if (!Instance)
		Instance = new ExternalPlayer();
}

void ExternalPlayer::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

ExternalPlayer * ExternalPlayer::instance()
{
	return Instance;
}

ExternalPlayer::ExternalPlayer()
{
	createDefaultConfiguration();
}

ExternalPlayer::~ExternalPlayer()
{
}

void ExternalPlayer::playSound(const QString &path)
{
	kdebugf();

	QString playerCommand = config_file.readEntry("Sounds", "SoundPlayer");
	QString volumeArguments;

	if (playerCommand.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: player path is empty\n");
		return;
	}

	QStringList argumentList;
	argumentList.append(path);

	QProcess process;
	process.start(playerCommand, argumentList);
	process.waitForFinished();
}

void ExternalPlayer::createDefaultConfiguration()
{
#ifdef Q_OS_MAC
	config_file.addVariable("Sounds", "SoundPlayer", "/Applications/Kadu.app/Contents/MacOS/playsound");
#else
	config_file.addVariable("Sounds", "SoundPlayer", "/usr/bin/play");
#endif
}

#include "moc_external-player.cpp"
