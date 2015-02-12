/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>

#include <phonon/mediaobject.h>
#include <phonon/phononnamespace.h>

#include "plugins/sound/sound-manager.h"

#include "phonon-player.h"

PhononPlayer * PhononPlayer::Instance = 0;

void PhononPlayer::createInstance()
{
	if (!Instance)
		Instance = new PhononPlayer();
}

void PhononPlayer::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

PhononPlayer * PhononPlayer::instance()
{
	return Instance;
}

PhononPlayer::PhononPlayer()
{
}

PhononPlayer::~PhononPlayer()
{
	if (Media)
	{
		Media->stop();
		Media->deleteLater();
	}
}

void PhononPlayer::playSound(const QString &path)
{
	if (Media)
		return;

	auto fileInfo = QFileInfo{path};
	if (!fileInfo.exists())
		return;

	Media = Phonon::createPlayer(Phonon::NotificationCategory);
	connect(Media, SIGNAL(finished()), Media, SLOT(deleteLater()));
	Media->setCurrentSource(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
	Media->play();
}

#include "moc_phonon-player.cpp"
