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
#include <qgrid.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qvbox.h>

#include "action.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "select_file.h"
#include "sound_slots.h"
#include "toolbar.h"

/**
 * @ingroup sound
 * @{
 */

SoundConfigurationWidget::SoundConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	soundFileSelectFile = new SelectFile("audio", this);
	QPushButton *testButton = new QPushButton(tr("Test"), this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addWidget(new QLabel(tr("Sound file") + ":", this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(soundFileSelectFile, 0, 1);
	gridLayout->addWidget(testButton, 0, 2);
}

SoundConfigurationWidget::~SoundConfigurationWidget()
{
}

void SoundConfigurationWidget::test()
{
	sound_manager->play(soundFileSelectFile->file(), true);
}

void SoundConfigurationWidget::saveNotifyConfigurations()
{
	if (currentNotifyEvent != "")
		soundFiles[currentNotifyEvent] = soundFileSelectFile->file();

	CONST_FOREACH(soundFile, soundFiles)
	{
		const QString &eventName = soundFile.key();
		config_file.writeEntry("Sounds", eventName + "_sound", *soundFile);
	}
}

void SoundConfigurationWidget::switchToEvent(const QString &event)
{
	if (currentNotifyEvent != "")
		soundFiles[currentNotifyEvent] = soundFileSelectFile->file();
	currentNotifyEvent = event;

	if (soundFiles.contains(event))
		soundFileSelectFile->setFile(soundFiles[event]);
	else
		soundFileSelectFile->setFile(config_file.readEntry("Sounds", event + "_sound"));
}

SoundSlots::SoundSlots(QObject *parent, const char *name) : QObject(parent, name),
	soundfiles(), soundNames(), soundTexts(), SamplePlayingTestMsgBox(0), SamplePlayingTestDevice(0),
	SamplePlayingTestSample(0), SampleRecordingTestMsgBox(0), SampleRecordingTestDevice(0),
	SampleRecordingTestSample(0), FullDuplexTestMsgBox(0), FullDuplexTestDevice(0), FullDuplexTestSample(0)
{
	kdebugf();

	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));

	mute_action = new Action("Unmute", tr("Mute sounds"), "muteSoundsAction", Action::TypeGlobal);
	mute_action->setOnShape("Mute", tr("Unmute sounds"));
	connect(mute_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(muteActionActivated(const UserGroup*, const QWidget*, bool)));
	connect(mute_action, SIGNAL(iconsRefreshed()), this, SLOT(setMuteActionState()));
	ToolBar::addDefaultAction("Kadu toolbar", "muteSoundsAction", 0);
	setMuteActionState();

	kdebugf2();
}

SoundSlots::~SoundSlots()
{
	kdebugf();
	delete mute_action;
	mute_action = 0;
	kdebugf2();
}

void SoundSlots::muteActionActivated(const UserGroup* /*users*/, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	sound_manager->setMute(is_on);
	mute_action->setAllOn(is_on);
	config_file.writeEntry("Sounds", "PlaySound", !is_on);
	kdebugf2();
}

void SoundSlots::setMuteActionState()
{
	mute_action->setAllOn(sound_manager->isMuted());
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	muteActionActivated(NULL, NULL, !sound_manager->isMuted());
	kdebugf2();
}

void SoundSlots::testSamplePlaying()
{
	kdebugf();
	if (SamplePlayingTestMsgBox != NULL)
		return;
	QString chatsound = config_file.readEntry("Sounds", "NewChat_sound");

	QFile file(chatsound);
	if (!file.open(IO_ReadOnly))
	{
		MessageBox::msg(tr("Opening test sample file failed."), false, "Warning");
		return;
	}
	// we are allocating 1 more word just in case of file.size() % sizeof(int16_t) != 0
	SamplePlayingTestSample = new int16_t[file.size() / sizeof(int16_t) + 1];
	if (file.readBlock((char*)SamplePlayingTestSample, file.size()) != (unsigned)file.size())
	{
		MessageBox::msg(tr("Reading test sample file failed."), false, "Warning");
		file.close();
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		return;
	}
	file.close();

	SamplePlayingTestDevice = sound_manager->openDevice(PLAY_ONLY, 11025);
	if (SamplePlayingTestDevice == NULL)
	{
		MessageBox::msg(tr("Opening sound device failed."), false, "Warning");
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		return;
	}

	sound_manager->enableThreading(SamplePlayingTestDevice);
	sound_manager->setFlushingEnabled(SamplePlayingTestDevice, true);
	connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(samplePlayingTestSamplePlayed(SoundDevice)));

	SamplePlayingTestMsgBox = new MessageBox(tr("Testing sample playing. You should hear some sound now."));
	SamplePlayingTestMsgBox->show();

	sound_manager->playSample(SamplePlayingTestDevice, SamplePlayingTestSample, file.size());
	kdebugf2();
}

void SoundSlots::samplePlayingTestSamplePlayed(SoundDevice device)
{
	kdebugf();
	if (device == SamplePlayingTestDevice)
	{
		disconnect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(samplePlayingTestSamplePlayed(SoundDevice)));
		sound_manager->closeDevice(device);
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		SamplePlayingTestMsgBox->deleteLater();
		SamplePlayingTestMsgBox = NULL;
	}
	kdebugf2();
}

void SoundSlots::testSampleRecording()
{
	kdebugf();
	if (SampleRecordingTestMsgBox != NULL)
		return;
	SampleRecordingTestDevice = sound_manager->openDevice(RECORD_ONLY, 8000);
	if (SampleRecordingTestDevice == NULL)
	{
		MessageBox::msg(tr("Opening sound device failed."), false, "Warning");
		return;
	}
	SampleRecordingTestSample = new int16_t[8000 * 3];//3 seconds of 16-bit sound with 8000Hz frequency

	sound_manager->enableThreading(SampleRecordingTestDevice);
	sound_manager->setFlushingEnabled(SampleRecordingTestDevice, true);
	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(sampleRecordingTestSampleRecorded(SoundDevice)));

	SampleRecordingTestMsgBox = new MessageBox(tr("Testing sample recording. Please talk now (3 seconds)."));
	SampleRecordingTestMsgBox->show();

	sound_manager->recordSample(SampleRecordingTestDevice, SampleRecordingTestSample, sizeof(int16_t) * 8000 * 3);
	kdebugf2();
}

void SoundSlots::sampleRecordingTestSampleRecorded(SoundDevice device)
{
	kdebugf();
	if (device == SampleRecordingTestDevice)
	{
		delete SampleRecordingTestMsgBox;
		SampleRecordingTestMsgBox = NULL;
		disconnect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(sampleRecordingTestSampleRecorded(SoundDevice)));

		sound_manager->closeDevice(device);
		SampleRecordingTestDevice = device = sound_manager->openDevice(PLAY_ONLY, 8000);
		if (device == NULL)
		{
			delete[] SampleRecordingTestSample;
			MessageBox::msg(tr("Cannot open sound device for playing!"), false, "Warning");
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: cannot open play device\n");
			return;
		}

		sound_manager->enableThreading(SampleRecordingTestDevice);
		sound_manager->setFlushingEnabled(SampleRecordingTestDevice, true);
		SampleRecordingTestMsgBox = new MessageBox(tr("You should hear your recorded sample now."));
		SampleRecordingTestMsgBox->show();

		connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(sampleRecordingTestSamplePlayed(SoundDevice)));

		sound_manager->playSample(device, SampleRecordingTestSample, sizeof(int16_t) * 8000 * 3);
	}
	kdebugf2();
}

void SoundSlots::sampleRecordingTestSamplePlayed(SoundDevice device)
{
	kdebugf();
	if (device == SampleRecordingTestDevice)
	{
		disconnect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(sampleRecordingTestSamplePlayed(SoundDevice)));
		sound_manager->closeDevice(device);
		delete[] SampleRecordingTestSample;
		SampleRecordingTestSample = NULL;
		SampleRecordingTestMsgBox->deleteLater();
		SampleRecordingTestMsgBox = NULL;
	}
	kdebugf2();
}

void SoundSlots::testFullDuplex()
{
	kdebugf();
	if (FullDuplexTestMsgBox != NULL)
		return;
	FullDuplexTestDevice = sound_manager->openDevice(PLAY_AND_RECORD, 8000);
	if (FullDuplexTestDevice == NULL)
	{
		MessageBox::msg(tr("Opening sound device failed."), false, "Warning");
		return;
	}
	FullDuplexTestSample = new int16_t[8000];

	sound_manager->enableThreading(FullDuplexTestDevice);
	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(fullDuplexTestSampleRecorded(SoundDevice)));

	FullDuplexTestMsgBox = new MessageBox(tr("Testing fullduplex. Please talk now.\nYou should here it with one second delay."), MessageBox::OK);
	connect(FullDuplexTestMsgBox, SIGNAL(okPressed()), this, SLOT(closeFullDuplexTest()));
	FullDuplexTestMsgBox->show();

	sound_manager->recordSample(FullDuplexTestDevice, FullDuplexTestSample, sizeof(int16_t) * 8000);
	kdebugf2();
}

void SoundSlots::fullDuplexTestSampleRecorded(SoundDevice device)
{
	kdebugf();
	if (device == FullDuplexTestDevice)
	{
		sound_manager->playSample(device, FullDuplexTestSample, sizeof(int16_t) * 8000);
		sound_manager->recordSample(device, FullDuplexTestSample, sizeof(int16_t) * 8000);
	}
	kdebugf2();
}

void SoundSlots::closeFullDuplexTest()
{
	kdebugf();
	disconnect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(fullDuplexTestSampleRecorded(SoundDevice)));
	sound_manager->closeDevice(FullDuplexTestDevice);
	delete[] FullDuplexTestSample;
	FullDuplexTestSample = NULL;
	FullDuplexTestMsgBox->deleteLater();
	FullDuplexTestMsgBox = NULL;
	kdebugf2();
}

/** @} */

