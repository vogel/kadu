/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qgrid.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qstring.h>
#include <qslider.h>

#include "chat.h"
#include "chat_manager.h"
#include "config_file.h"
// #include "config_dialog.h"
#include "debug.h"
#include "sound.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "../notify/notify.h"
#include "sound_file.h"

#include "sound_slots.h"

/**
 * @ingroup sound
 * @{
 */
SoundManager* sound_manager=NULL;
SoundSlots* sound_slots;

extern "C" int sound_init()
{
	kdebugf();
	new SoundManager("sounds", "sound.conf");
	kdebugf2();
	return 0;
}

extern "C" void sound_close()
{
	kdebugf();
	delete sound_manager;
	sound_manager=NULL;
	kdebugf2();
}

SamplePlayThread::SamplePlayThread(SoundDevice device)
	: Device(device), Sample(0), SampleLen(0), Stopped(false),
	PlayingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	PlayingSemaphore++;
	kdebugf2();
}

void SamplePlayThread::run()
{
	kdebugf();
	for(;;)
	{
		PlayingSemaphore++;
		if (Stopped)
		{
			SampleSemaphore--;
			break;
		}
		bool result;
		emit sound_manager->playSampleImpl(Device, Sample, SampleLen, result);
		QApplication::postEvent(this, new QCustomEvent(QEvent::User, Device));
		SampleSemaphore--;
	}
	kdebugf2();
}

void SamplePlayThread::customEvent(QCustomEvent* event)
{
	if (event->type() == QEvent::User)
		emit samplePlayed((SoundDevice)event->data());
}

void SamplePlayThread::playSample(const int16_t* data, int length)
{
	kdebugf();
	SampleSemaphore++;
	Sample = data;
	SampleLen = length;
	PlayingSemaphore--;
	kdebugf2();
}

void SamplePlayThread::stop()
{
	kdebugf();
	SampleSemaphore++;
	Stopped = true;
	PlayingSemaphore--;
	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SamplePlayThread\n");
		this->terminate();
		wait(1000);
	}
	kdebugf2();
}

SampleRecordThread::SampleRecordThread(SoundDevice device)
	: Device(device), Sample(0), SampleLen(0), Stopped(false),
	RecordingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	RecordingSemaphore++;
	kdebugf2();
}

void SampleRecordThread::run()
{
	kdebugf();
	for(;;)
	{
		RecordingSemaphore++;
		if (Stopped)
		{
			SampleSemaphore--;
			break;
		}
		bool result;
		emit sound_manager->recordSampleImpl(Device, Sample, SampleLen, result);
		QApplication::postEvent(this, new QCustomEvent(QEvent::User, Device));
		SampleSemaphore--;
	}
	kdebugf2();
}

void SampleRecordThread::customEvent(QCustomEvent* event)
{
	if (event->type() == QEvent::User)
		emit sampleRecorded((SoundDevice)event->data());
}

void SampleRecordThread::recordSample(int16_t* data, int length)
{
	kdebugf();
	SampleSemaphore++;
	Sample = data;
	SampleLen = length;
	RecordingSemaphore--;
	kdebugf2();
}

void SampleRecordThread::stop()
{
	kdebugf();
	SampleSemaphore++;
	Stopped = true;
	RecordingSemaphore--;
	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SampleRecordThread\n");
		this->terminate();
		wait(1000);
	}
	kdebugf2();
}

SoundManager::SoundManager(const QString& name, const QString& configname) : Notifier(),
	themes(new Themes(name, configname, "sound_manager")),
	lastsoundtime(), mute(false), PlayingThreads(), RecordingThreads(),
	play_thread(new SoundPlayThread()), simple_player_count(0)
{
	kdebugf();

	import_0_5_0_configuration();

	lastsoundtime.start();

	play_thread->start();

// 	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default","Sounds"), "SoundsTab");
// 	ConfigDialog::addCheckBox("Sounds", "Sounds",
// 			QT_TRANSLATE_NOOP("@default","Play sounds"), "PlaySound", false);

	bool volCntrl = true;
#ifdef Q_OS_MACX
	volCntrl = false;
#endif
// 	ConfigDialog::addCheckBox("Sounds", "Sounds",
// 			QT_TRANSLATE_NOOP("@default","Enable volume control (player must support it)"),
// 			"VolumeControl", volCntrl);
// 	ConfigDialog::addGrid("Sounds", "Sounds", "volume", 2);
// 	ConfigDialog::addLabel("Sounds", "volume", QT_TRANSLATE_NOOP("@default","Volume"));
// 	ConfigDialog::addSlider("Sounds", "volume", "slider", "SoundVolume", 0, 100, 20, 50);

// 	ConfigDialog::addHBox("Sounds", "Sounds", "sound_theme");
// 	ConfigDialog::addComboBox("Sounds", "sound_theme",
// 			QT_TRANSLATE_NOOP("@default","Sound theme"));
// 	ConfigDialog::addSelectPaths("Sounds", "sound_theme",
// 			QT_TRANSLATE_NOOP("@default","Sound paths"));

// 	ConfigDialog::addHBox("Sounds", "Sounds", "sound_box");
// 	ConfigDialog::addListView("Sounds", "sound_box", "sound_files");
// 	ConfigDialog::addVBox("Sounds", "sound_box", "util_box");
// 	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Choose"));
// 	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Clear"));
// 	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Test"));

// 	ConfigDialog::addHGroupBox("Sounds", "Sounds", QT_TRANSLATE_NOOP("@default","Samples"));
// 	ConfigDialog::addPushButton("Sounds", "Samples", QT_TRANSLATE_NOOP("@default","Test sample playing"));
// 	ConfigDialog::addPushButton("Sounds", "Samples", QT_TRANSLATE_NOOP("@default","Test sample recording"));
// 	ConfigDialog::addPushButton("Sounds", "Samples", QT_TRANSLATE_NOOP("@default","Test full duplex"));

	sound_manager = this;
	sound_slots = new SoundSlots(this, "sound_slots");

// 	ConfigDialog::registerSlotOnCreateTab("Sounds", sound_slots, SLOT(onCreateTabSounds()));
// 	ConfigDialog::registerSlotOnApplyTab("Sounds", sound_slots, SLOT(onApplyTabSounds()));
// 	ConfigDialog::connectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), sound_slots, SLOT(soundPlayer(bool)));
// 	ConfigDialog::connectSlot("Sounds", "Choose", SIGNAL(clicked()), sound_slots, SLOT(chooseSoundFile()));
// 	ConfigDialog::connectSlot("Sounds", "Clear", SIGNAL(clicked()), sound_slots, SLOT(clearSoundFile()));
// 	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(clicked()), sound_slots, SLOT(testSoundFile()));
// 	ConfigDialog::connectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), sound_slots, SLOT(chooseSoundTheme(const QString&)));
// 	ConfigDialog::connectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), sound_slots, SLOT(selectedPaths(const QStringList&)));
// 	ConfigDialog::connectSlot("Sounds", "Test sample playing", SIGNAL(clicked()), sound_slots, SLOT(testSamplePlaying()));
// 	ConfigDialog::connectSlot("Sounds", "Test sample recording", SIGNAL(clicked()), sound_slots, SLOT(testSampleRecording()));
// 	ConfigDialog::connectSlot("Sounds", "Test full duplex", SIGNAL(clicked()), sound_slots, SLOT(testFullDuplex()));

	config_file.addVariable("Sounds", "SoundTheme", "default");
	config_file.addVariable("Sounds", "SoundPaths", QString::null);

	themes->setPaths(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	themes->setTheme(config_file.readEntry("Sounds","SoundTheme"));

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Sound"), this);

	config_file.addVariable("Notify", "ConnectionError_Sound", true);
	config_file.addVariable("Notify", "NewChat_Sound", true);
	config_file.addVariable("Notify", "NewMessage_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToBusy_Sound", true);

	kdebugf2();
}

SoundManager::~SoundManager()
{
	kdebugf();
	play_thread->endThread();

// 	ConfigDialog::unregisterSlotOnCreateTab("Sounds", sound_slots, SLOT(onCreateTabSounds()));
// 	ConfigDialog::unregisterSlotOnApplyTab("Sounds", sound_slots, SLOT(onApplyTabSounds()));
// 	ConfigDialog::disconnectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), sound_slots, SLOT(soundPlayer(bool)));
// 	ConfigDialog::disconnectSlot("Sounds", "Choose", SIGNAL(clicked()), sound_slots, SLOT(chooseSoundFile()));
// 	ConfigDialog::disconnectSlot("Sounds", "Clear", SIGNAL(clicked()), sound_slots, SLOT(clearSoundFile()));
// 	ConfigDialog::disconnectSlot("Sounds", "Test", SIGNAL(clicked()), sound_slots, SLOT(testSoundFile()));
// 	ConfigDialog::disconnectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), sound_slots, SLOT(chooseSoundTheme(const QString&)));
// 	ConfigDialog::disconnectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), sound_slots, SLOT(selectedPaths(const QStringList&)));
// 	ConfigDialog::disconnectSlot("Sounds", "Test sample playing", SIGNAL(clicked()), sound_slots, SLOT(testSamplePlaying()));
// 	ConfigDialog::disconnectSlot("Sounds", "Test sample recording", SIGNAL(clicked()), sound_slots, SLOT(testSampleRecording()));
// 	ConfigDialog::disconnectSlot("Sounds", "Test full duplex", SIGNAL(clicked()), sound_slots, SLOT(testFullDuplex()));

	delete sound_slots;
	sound_slots = NULL;

// 	ConfigDialog::removeControl("Sounds", "Test full duplex");
// 	ConfigDialog::removeControl("Sounds", "Test sample recording");
// 	ConfigDialog::removeControl("Sounds", "Test sample playing");
// 	ConfigDialog::removeControl("Sounds", "Samples");
// 	ConfigDialog::removeControl("Sounds", "Test");
// 	ConfigDialog::removeControl("Sounds", "Clear");
// 	ConfigDialog::removeControl("Sounds", "Choose");
// 	ConfigDialog::removeControl("Sounds", "util_box");
// 	ConfigDialog::removeControl("Sounds", "sound_files");
// 	ConfigDialog::removeControl("Sounds", "sound_box");
// 	ConfigDialog::removeControl("Sounds", "Sound paths");
// 	ConfigDialog::removeControl("Sounds", "Sound theme");
// 	ConfigDialog::removeControl("Sounds", "sound_theme");
// 	ConfigDialog::removeControl("Sounds", "slider");
// 	ConfigDialog::removeControl("Sounds", "Volume");
// 	ConfigDialog::removeControl("Sounds", "volume");
// 	ConfigDialog::removeControl("Sounds", "Enable volume control (player must support it)");
// 	ConfigDialog::removeControl("Sounds", "Play sounds");
// 	ConfigDialog::removeTab("Sounds");

	notification_manager->unregisterNotifier("Sound");

	play_thread->wait(2000);
	if (play_thread->running())
	{
		kdebugm(KDEBUG_WARNING, "terminating play_thread!\n");
		play_thread->terminate();
	}
	delete play_thread;
	delete themes;

	kdebugf2();
}

void SoundManager::import_0_5_0_configuration()
{
	if (config_file.readEntry("Sounds", "StatusChanged/ToOnline_sound", "foobar") == "foobar")
	{
		config_file.writeEntry("Sounds", "StatusChanged/ToOnline_sound", config_file.readEntry("Sounds", "StatusAvailable_sound"));
		config_file.writeEntry("Sounds", "StatusChanged/ToBusy_sound", config_file.readEntry("Sounds", "StatusBusy_sound"));
		config_file.writeEntry("Sounds", "StatusChanged/ToInvisible_sound", config_file.readEntry("Sounds", "StatusInvisible_sound"));
		config_file.writeEntry("Sounds", "StatusChanged/ToOffline_sound", config_file.readEntry("Sounds", "StatusNotAvailable_sound"));

		config_file.writeEntry("Sounds", "NewChat_sound", config_file.readEntry("Sounds", "Chat_sound"));
		config_file.writeEntry("Sounds", "NewMessage_sound", config_file.readEntry("Sounds", "Message_sound"));
	}
}

Themes *SoundManager::theme()
{
	return themes;
}

bool SoundManager::isMuted() const
{
	return mute;
}

void SoundManager::setMute(const bool& enable)
{
	mute = enable;
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

	QString sound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		sound = config_file.readEntry("Sounds", soundName + "_sound");
	else
		sound = themes->themePath(config_file.readEntry("Sounds", "SoundTheme")) + themes->getThemeEntry(soundName);

	if (QFile::exists(sound))
	{
		play(sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0 * config_file.readDoubleNumEntry("Sounds", "SoundVolume") / 100);
		lastsoundtime.restart();
	}
	else
		fprintf(stderr, "file (%s) not found\n", sound.local8Bit().data());
}

void SoundManager::notify(Notification *notification)
{
	kdebugf();

	playSound(notification->type());

	kdebugf2();
}

void SoundManager::play(const QString &path, bool force)
{
	kdebugf();
	bool volCntrl;
	float vol;

	if (isMuted() && !force)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: muted\n");
		return;
	}
// 	if (ConfigDialog::dialogOpened())
// 	{
// 		volCntrl=ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)")->isChecked();
// 		vol=1.0*ConfigDialog::getSlider("Sounds", "slider")->value()/100;
// 	}
// 	else
// 	{
// 		volCntrl=config_file.readBoolEntry("Sounds","VolumeControl");
// 		vol=1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100;
// 	}
	if (QFile::exists(path))
		play(path, volCntrl, vol);
	else
		fprintf(stderr, "file (%s) not found\n", path.local8Bit().data());
	kdebugf2();
}

int SoundManager::timeAfterLastSound() const
{
	return lastsoundtime.elapsed();
}

SoundDevice SoundManager::openDevice(SoundDeviceType type, int sample_rate, int channels)
{
	kdebugf();
	SoundDevice device = NULL;
	emit openDeviceImpl(type, sample_rate, channels, device);
	kdebugf2();
	return device;
}

void SoundManager::closeDevice(SoundDevice device)
{
	kdebugf();
	if (PlayingThreads.contains(device))
	{
		SamplePlayThread* playing_thread = PlayingThreads[device];
		disconnect(playing_thread, SIGNAL(samplePlayed(SoundDevice)), this, SIGNAL(samplePlayed(SoundDevice)));
		playing_thread->stop();
		PlayingThreads.remove(device);
		delete playing_thread;
	}
	if (RecordingThreads.contains(device))
	{
		SampleRecordThread* recording_thread = RecordingThreads[device];
		disconnect(recording_thread, SIGNAL(sampleRecorded(SoundDevice)), this, SIGNAL(sampleRecorded(SoundDevice)));
		recording_thread->stop();
		RecordingThreads.remove(device);
		delete recording_thread;
	}
	emit closeDeviceImpl(device);

	kdebugf2();
}

void SoundManager::enableThreading(SoundDevice device)
{
	kdebugf();
	if (!PlayingThreads.contains(device))
	{
		SamplePlayThread* playing_thread = new SamplePlayThread(device);
		connect(playing_thread, SIGNAL(samplePlayed(SoundDevice)), this, SIGNAL(samplePlayed(SoundDevice)));
		playing_thread->start();
		PlayingThreads.insert(device, playing_thread);
	}
	if (!RecordingThreads.contains(device))
	{
		SampleRecordThread* recording_thread = new SampleRecordThread(device);
		connect(recording_thread, SIGNAL(sampleRecorded(SoundDevice)), this, SIGNAL(sampleRecorded(SoundDevice)));
		recording_thread->start();
		RecordingThreads.insert(device, recording_thread);
	}
	kdebugf2();
}

void SoundManager::setFlushingEnabled(SoundDevice device, bool enabled)
{
	kdebugf();
	emit setFlushingEnabledImpl(device, enabled);
	kdebugf2();
}

bool SoundManager::playSample(SoundDevice device, const int16_t* data, int length)
{
	kdebugf();
	bool result;
	if (PlayingThreads.contains(device))
	{
		PlayingThreads[device]->playSample(data, length);
		result = true;
	}
	else
		emit playSampleImpl(device, data, length, result);
	kdebugf2();
	return result;
}

bool SoundManager::recordSample(SoundDevice device, int16_t* data, int length)
{
	kdebugf();
	bool result;
	if (RecordingThreads.contains(device))
	{
		RecordingThreads[device]->recordSample(data, length);
		result = true;
	}
	else
		emit recordSampleImpl(device, data, length, result);
	kdebugf2();
	return result;
}

void SoundManager::connectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>>%s %s\n", signal, SIGNAL(playSound(const QString&,bool,double)) );
	if (strcmp(signal,SIGNAL(playSound(const QString&,bool,double)))==0)
		++simple_player_count;
}

void SoundManager::disconnectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>>%s %s\n", signal, SIGNAL(playSound(const QString&,bool,double)) );
	if (strcmp(signal,SIGNAL(playSound(const QString&,bool,double)))==0)
		--simple_player_count;
}

void SoundManager::play(const QString &path, bool volCntrl, double vol)
{
	kdebugf();
	if (simple_player_count>0)
		emit playSound(path, volCntrl, vol);
	else
		play_thread->tryPlay(path.local8Bit().data(), volCntrl, vol);
	kdebugf2();
}

SoundPlayThread::SoundPlayThread() : QThread(),
	mutex(), semaphore(new QSemaphore(100)), end(false), list()
{
	(*semaphore) += 100;
}

SoundPlayThread::~SoundPlayThread()
{
	delete semaphore;
}

void SoundPlayThread::tryPlay(const char *path, bool volumeControl, float volume)
{
	kdebugf();
	if (mutex.tryLock())
	{
		list.push_back(SndParams(path, volumeControl, volume));
		mutex.unlock();
		(*semaphore)--;
	}
	kdebugf2();
}

void SoundPlayThread::run()
{
	kdebugf();
	while (!end)
	{
		(*semaphore)++;
		mutex.lock();
		kdebugmf(KDEBUG_INFO, "locked\n");
		if (end)
		{
			mutex.unlock();
			break;
		}
		SndParams params=list.first();
		list.pop_front();

		play(params.filename.local8Bit().data(),
				params.volumeControl, params.volume);
		mutex.unlock();
		kdebugmf(KDEBUG_INFO, "unlocked\n");
	}//end while(!end)
	kdebugf2();
}

bool SoundPlayThread::play(const char *path, bool volumeControl, float volume)
{
	bool ret=false;
	SoundFile *sound=new SoundFile(path);

	if (!sound->isOk())
	{
		fprintf(stderr, "broken sound file?\n");
		delete sound;
		return false;
	}

	kdebugm(KDEBUG_INFO, "\n");
	kdebugm(KDEBUG_INFO, "length:   %d\n", sound->length);
	kdebugm(KDEBUG_INFO, "speed:    %d\n", sound->speed);
	kdebugm(KDEBUG_INFO, "channels: %d\n", sound->channels);

	if (volumeControl)
		sound->setVolume(volume);

	SoundDevice dev;
	dev = sound_manager->openDevice(PLAY_ONLY, sound->speed, sound->channels);
	sound_manager->setFlushingEnabled(dev, true);
	ret = sound_manager->playSample(dev, sound->data, sound->length*sizeof(sound->data[0]));
	sound_manager->closeDevice(dev);

	delete sound;
	return ret;
}

void SoundPlayThread::endThread()
{
	mutex.lock();
	end = true;
	mutex.unlock();
	(*semaphore)--;
}

SndParams::SndParams(QString fm, bool volCntrl, float vol) :
			filename(fm), volumeControl(volCntrl), volume(vol)
{
}

SndParams::SndParams(const SndParams &p) : filename(p.filename),
						volumeControl(p.volumeControl), volume(p.volume)
{
}

/** @} */

