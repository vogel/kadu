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
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMenuBar>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/select-file.h"
#include "gui/windows/kadu-window.h"

#include "debug.h"
#include "icons-manager.h"

#include "sound-slots.h"

/**
 * @ingroup sound
 * @{
 */

SoundConfigurationWidget::SoundConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent), currentNotifyEvent("")
{
    	QPushButton *testButton = new QPushButton(IconsManager::instance()->loadIcon("MediaPlayerButton"),"", this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	soundFileSelectFile = new SelectFile("audio", this);
	connect(soundFileSelectFile, SIGNAL(fileChanged()), this, SIGNAL(soundFileEdited()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->insertSpacing(0, 20);
	layout->addWidget(testButton);
	layout->addWidget(soundFileSelectFile);

	dynamic_cast<NotifyGroupBox *>(parent)->addWidget(this);
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

	foreach (const QString &key, soundFiles.keys())
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
	if (index == 0)
		return;

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

	mute_action = new ActionDescription(this,
		ActionDescription::TypeGlobal, "muteSoundsAction",
		this, SLOT(muteActionActivated(QAction *, bool)),
		"Unmute", tr("Mute sounds"), true, tr("Unmute sounds")
	);
	connect(mute_action, SIGNAL(actionCreated(Action *)), this, SLOT(setMuteActionState()));

	if (firstLoad)
		KaduWindow::addAction("muteSoundsAction");

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
	foreach (Action *action, mute_action->actions())
		action->setChecked(is_on);
	config_file.writeEntry("Sounds", "PlaySound", !is_on);
	kdebugf2();
}

void SoundSlots::setMuteActionState()
{
	foreach (Action *action, mute_action->actions())
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
		MessageDialog::msg(tr("Opening test sample file failed."), false, "Warning");
		return;
	}
	// we are allocating 1 more word just in case of file.size() % sizeof(qint16) != 0
	SamplePlayingTestSample = new qint16[file.size() / sizeof(qint16) + 1];
	if (file.readBlock((char*)SamplePlayingTestSample, file.size()) != (unsigned)file.size())
	{
		MessageDialog::msg(tr("Reading test sample file failed."), false, "Warning");
		file.close();
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		return;
	}
	file.close();

	SamplePlayingTestDevice = sound_manager->openDevice(PLAY_ONLY, 11025);
	if (SamplePlayingTestDevice == NULL)
	{
		MessageDialog::msg(tr("Opening sound device failed."), false, "Warning");
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		return;
	}

	sound_manager->enableThreading(SamplePlayingTestDevice);
	sound_manager->setFlushingEnabled(SamplePlayingTestDevice, true);
	connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(samplePlayingTestSamplePlayed(SoundDevice)));

	SamplePlayingTestMsgBox = new MessageDialog(tr("Testing sample playing. You should hear some sound now."));
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
	SampleRecordingTestDevice = sound_manager->openDevice(SoundDeviceRecordOnly, 8000);
	if (SampleRecordingTestDevice == NULL)
	{
		MessageDialog::msg(tr("Opening sound device failed."), false, "Warning");
		return;
	}
	SampleRecordingTestSample = new qint16[8000 * 3];//3 seconds of 16-bit sound with 8000Hz frequency

	sound_manager->enableThreading(SampleRecordingTestDevice);
	sound_manager->setFlushingEnabled(SampleRecordingTestDevice, true);
	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(sampleRecordingTestSampleRecorded(SoundDevice)));

	SampleRecordingTestMsgBox = new MessageDialog(tr("Testing sample recording. Please talk now (3 seconds)."));
	SampleRecordingTestMsgBox->show();

	sound_manager->recordSample(SampleRecordingTestDevice, SampleRecordingTestSample, sizeof(qint16) * 8000 * 3);
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
		SampleRecordingTestDevice = device = sound_manager->openDevice(SoundDevicePlayOnly, 8000);
		if (device == NULL)
		{
			delete[] SampleRecordingTestSample;
			MessageDialog::msg(tr("Cannot open sound device for playing!"), false, "Warning");
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: cannot open play device\n");
			return;
		}

		sound_manager->enableThreading(SampleRecordingTestDevice);
		sound_manager->setFlushingEnabled(SampleRecordingTestDevice, true);
		SampleRecordingTestMsgBox = new MessageDialog(tr("You should hear your recorded sample now."));
		SampleRecordingTestMsgBox->show();

		connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(sampleRecordingTestSamplePlayed(SoundDevice)));

		sound_manager->playSample(device, SampleRecordingTestSample, sizeof(qint16) * 8000 * 3);
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
	FullDuplexTestDevice = sound_manager->openDevice(SoundDevicePlayAndRecord, 8000);
	if (FullDuplexTestDevice == NULL)
	{
		MessageDialog::msg(tr("Opening sound device failed."), false, "Warning");
		return;
	}
	FullDuplexTestSample = new qint16[8000];

	sound_manager->enableThreading(FullDuplexTestDevice);
	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(fullDuplexTestSampleRecorded(SoundDevice)));

	FullDuplexTestMsgBox = new MessageDialog(tr("Testing fullduplex. Please talk now.\nYou should hear it with one second delay."), MessageDialog::OK);
	connect(FullDuplexTestMsgBox, SIGNAL(okPressed()), this, SLOT(closeFullDuplexTest()));
	FullDuplexTestMsgBox->show();

	sound_manager->recordSample(FullDuplexTestDevice, FullDuplexTestSample, sizeof(qint16) * 8000);
	kdebugf2();
}

void SoundSlots::fullDuplexTestSampleRecorded(SoundDevice device)
{
	kdebugf();
	if (device == FullDuplexTestDevice)
	{
		sound_manager->playSample(device, FullDuplexTestSample, sizeof(qint16) * 8000);
		sound_manager->recordSample(device, FullDuplexTestSample, sizeof(qint16) * 8000);
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

