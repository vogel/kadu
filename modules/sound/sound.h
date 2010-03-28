/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2003, 2004 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef SOUND_H
#define SOUND_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>

#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "notify/notifier.h"
#include "modules.h"
#include "themes.h"

#include "sound-exports.h"

class Notification;
class PathListEdit;
class SamplePlayThread;
class SampleRecordThread;
class SoundConfigurationWidget;
class SoundPlayer;
class SoundPlayThread;

/**
 * @defgroup sound Sound
 * The sound module.
 * @{
 */

typedef void *SoundDevice;

/**
**/
enum SoundDeviceType
{
	SoundDevicePlayOnly
};


class SOUNDAPI SoundManager : public QObject
{
    Q_OBJECT
	
	friend class SamplePlayThread;

	SoundPlayer *Player;

	SoundConfigurationWidget *ConfigurationWidget;

	QTime LastSoundTime;
	bool Mute;

	SoundPlayThread *PlayThread;

	int SimplePlayerCount;

	void connectNotify(const char *signal);
	void disconnectNotify(const char *signal);

	void import_0_6_5_configuration();
	void createDefaultConfiguration();

public slots:
	void play(const QString &path, bool force = false);
	void play(const QString &path, bool volumeControl, double volume);
	void setMute(const bool& enable);

public:
	SoundManager(bool firstLoad);
	virtual ~SoundManager();

	void setPlayer(SoundPlayer *player);

	void playSound(const QString &soundName);

	bool isMuted() const;
	int timeAfterLastSound() const;

};

extern SOUNDAPI SoundManager *sound_manager;

class SoundSlots;
extern SOUNDAPI SoundSlots *sound_slots;
/** @} */

#endif
