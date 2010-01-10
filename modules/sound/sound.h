/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	SoundDeviceRecordOnly,
	SoundDevicePlayOnly,
	SoundDevicePlayAndRecord
};


class SOUNDAPI SoundManager : public Notifier, public ConfigurationUiHandler
{
    Q_OBJECT
	
	friend class SamplePlayThread;
	friend class SampleRecordThread;

	SoundPlayer *Player;

	Themes *MyThemes;
	ConfigComboBox *ThemesComboBox;
	PathListEdit *ThemesPaths;
	SoundConfigurationWidget *ConfigurationWidget;

	QTime LastSoundTime;
	bool Mute;

	QMap<SoundDevice, SamplePlayThread *> PlayingThreads;
	QMap<SoundDevice, SampleRecordThread *> RecordingThreads;
	SoundPlayThread *PlayThread;

	int SimplePlayerCount;

	void connectNotify(const char *signal);
	void disconnectNotify(const char *signal);

	void applyTheme(const QString &themeName);
	void playSound(const QString &soundName);

	void import_0_6_5_configuration();
	void createDefaultConfiguration();

private slots:
	void setSoundThemes();
	void configurationWindowApplied();
	void soundFileEdited();

public slots:
	void play(const QString &path, bool force = false);
	void play(const QString &path, bool volumeControl, double volume);
	bool playSampleSlot(SoundDevice device, const qint16 *data, int length);
	bool recordSampleSlot(SoundDevice device, qint16 *data, int length);
	void setMute(const bool& enable);
	void stop();

public:
	SoundManager(bool firstLoad, const QString &name, const QString &configname);
	virtual ~SoundManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);

	virtual void notify(Notification *notification);

	void setPlayer(SoundPlayer *player);

	Themes * theme();

	bool isMuted() const;
	int timeAfterLastSound() const;

	SoundDevice openDevice(SoundDeviceType type, int sampleRate, int channels = 1);
	void closeDevice(SoundDevice device);
	void enableThreading(SoundDevice device);
	void setFlushingEnabled(SoundDevice device, bool enabled);
	bool playSample(SoundDevice device, const qint16 *data, int length);
	bool recordSample(SoundDevice device, qint16 *data, int length);

signals:
	void samplePlayed(SoundDevice device);
	void sampleRecorded(SoundDevice device);

};

extern SOUNDAPI SoundManager *sound_manager;
/** @} */

#endif
