/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef PHONON_SOUND_H
#define PHONON_SOUND_H

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QWaitCondition>

#include "plugins/sound/sound-player.h"

namespace Phonon
{
	class AudioOutput;
	class MediaObject;
}

class PhononPlayer : public SoundPlayer
{
	Q_OBJECT
	Q_DISABLE_COPY(PhononPlayer)

	static PhononPlayer * Instance;

	Phonon::MediaObject *Media;

	PhononPlayer();
	virtual ~PhononPlayer();

	QMutex MediaObjectMutex;
	QWaitCondition MediaObjectCreation;

private slots:
	void createMediaObject();

public:
	static void createInstance();
	static void destroyInstance();
	static PhononPlayer * instance();

	virtual void playSound(const QString &path);

signals:
	void createRequest();
};

#endif // PHONON_SOUND_H
