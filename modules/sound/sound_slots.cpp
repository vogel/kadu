/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMenuBar>
#include <QtGui/QVBoxLayout>

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
	warning = new QLabel("<b>" + tr("Choose 'Custom' theme in 'Sound' page to change sound file") + "</b>", this);
	soundFileSelectFile = new SelectFile("audio", this);
	QPushButton *testButton = new QPushButton(tr("Test"), this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	QGridLayout *gridLayout = new QGridLayout(this);
 	gridLayout->addMultiCellWidget(warning, 0, 0, 0, 3);
	gridLayout->addWidget(new QLabel(tr("Sound file") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(soundFileSelectFile, 1, 1);
	gridLayout->addWidget(testButton, 1, 2);
	
	parent->layout()->addWidget(this);
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

	foreach(const QString &key, soundFiles.keys())
		config_file.writeEntry("Sounds", key + "_sound", soundFiles[key]);
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

void SoundConfigurationWidget::themeChanged(int index)
{
	warning->setShown(index != 0);
	soundFileSelectFile->setEnabled(index == 0);

	//refresh soundFiles
	foreach (const QString &key, soundFiles.keys())
	{
		soundFiles[key] = config_file.readEntry("Sounds", key + "_sound");
		if (key == currentNotifyEvent)
			soundFileSelectFile->setFile(soundFiles[key]);
	}
}

SoundSlots::SoundSlots(bool firstLoad, QObject *parent)
	: QObject(parent),
	soundfiles(), soundNames(), soundTexts(), SamplePlayingTestMsgBox(0), SamplePlayingTestDevice(0),
	SamplePlayingTestSample(0), SampleRecordingTestMsgBox(0), SampleRecordingTestDevice(0),
	SampleRecordingTestSample(0), FullDuplexTestMsgBox(0), FullDuplexTestDevice(0), FullDuplexTestSample(0)
{
	kdebugf();

	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));

	mute_action = new ActionDescription(
		ActionDescription::TypeGlobal, "muteSoundsAction",
		this, SLOT(muteActionActivated(QAction *, bool)),
		"Unmute", tr("Mute sounds"), true, tr("Unmute sounds")
	);
	connect(mute_action, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setMuteActionState()));

	if (firstLoad)
		Kadu::addAction("muteSoundsAction");

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

void SoundSlots::themeChanged(const QString &theme)
{
	sound_manager->theme()->setTheme(theme);
}

void SoundSlots::muteActionActivated(QAction  *action, bool is_on)
{
	Q_UNUSED(action)
	kdebugf();
	sound_manager->setMute(is_on);
 	foreach (KaduAction *action, mute_action->actions())
		action->setChecked(is_on);
	config_file.writeEntry("Sounds", "PlaySound", !is_on);
	kdebugf2();
}

void SoundSlots::setMuteActionState()
{
 	foreach (KaduAction *action, mute_action->actions())
		action->setChecked(sound_manager->isMuted());
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	muteActionActivated(NULL, !sound_manager->isMuted());
	kdebugf2();
}

void SoundSlots::configurationUpdated()
{
	muteActionActivated(0, !config_file.readBoolEntry("Sounds", "PlaySound"));
}

void SoundSlots::testSamplePlaying()
{
	kdebugf();
	if (SamplePlayingTestMsgBox != NULL)
		return;

	QString chatsound = sound_manager->theme()->themePath() + sound_manager->theme()->getThemeEntry("NewChat");

	/* Dorr: I know that this brokes the test sample idea but
	   at least there are no noises when playing sample songs
	   from sound themes.
	*/
#if 1
	sound_manager->play(chatsound, true);
#else
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
#endif
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

