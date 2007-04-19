/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qcheckbox.h>
#include <qlistview.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qgrid.h>
#include <qmenubar.h>

#include "action.h"
#include "sound_slots.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "config_dialog.h"

/**
 * @ingroup sound
 * @{
 */
SoundSlots::SoundSlots(QObject *parent, const char *name) : QObject(parent, name),
	muteitem(0), soundfiles(), soundNames(), soundTexts(),
	SamplePlayingTestMsgBox(0), SamplePlayingTestDevice(0), SamplePlayingTestSample(0),
	SampleRecordingTestMsgBox(0), SampleRecordingTestDevice(0), SampleRecordingTestSample(0),
	FullDuplexTestMsgBox(0), FullDuplexTestDevice(0), FullDuplexTestSample(0)
{
	kdebugf();

	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));

	mute_action = new Action(icons_manager->loadIcon("Unmute"),
		tr("Mute sounds"), "muteSoundsAction", Action::TypeGlobal);
	mute_action->setOnShape(icons_manager->loadIcon("Mute"), tr("Unmute sounds"));
	mute_action->setAllOn(sound_manager->isMuted());
	connect(mute_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(muteActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("muteSoundsAction", mute_action);
	KaduActions.addDefaultToolbarAction("Kadu toolbar", "muteSoundsAction", 0);

	kdebugf2();
}

SoundSlots::~SoundSlots()
{
	kdebugf();
	kadu->mainMenu()->removeItem(muteitem);
	KaduActions.remove("muteSoundsAction");
	kdebugf2();
}

void SoundSlots::onCreateTabSounds()
{
	kdebugf();

	soundTexts.clear();
	soundNames.clear();

	CONST_FOREACH(notifyEvent, notification_manager->notifyEvents())
	{
		soundTexts << tr((*notifyEvent).description);
		soundNames << tr((*notifyEvent).name);
	}

	QCheckBox *b_playsound= ConfigDialog::getCheckBox("Sounds", "Play sounds");
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QGrid *g_volume= ConfigDialog::getGrid("Sounds","volume");
	QComboBox *cb_soundtheme= ConfigDialog::getComboBox("Sounds", "Sound theme");
	cb_soundtheme->insertItem("Custom");// 0 position
	cb_soundtheme->insertStringList(sound_manager->theme()->themes());
	cb_soundtheme->setCurrentText(config_file.readEntry("Sounds", "SoundTheme"));
	cb_soundtheme->changeItem(tr("Custom"), 0);// and now add translation
	if (sound_manager->theme()->themes().contains("default"))
		cb_soundtheme->changeItem(tr("default"), sound_manager->theme()->themes().findIndex("default")+1);

	QHBox* box=ConfigDialog::getHBox("Sounds","sound_box");
	QHBox* soundtheme=ConfigDialog::getHBox("Sounds", "sound_theme");
	box->setEnabled(b_playsound->isChecked());
	soundtheme->setEnabled(b_playsound->isChecked());

	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds","sound_files");
	lv_soundfiles->setSorting(-1);
	lv_soundfiles->addColumn(tr("Event"));
	lv_soundfiles->addColumn(tr("Sound file"));
	lv_soundfiles->setAllColumnsShowFocus(true);
	lv_soundfiles->setColumnWidthMode(0, QListView::Maximum);
	lv_soundfiles->setColumnWidthMode(1, QListView::Maximum);
	lv_soundfiles->setResizeMode(QListView::LastColumn);
	lv_soundfiles->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored));
	config_file.addVariable("Sounds", "Notify_sound", config_file.readEntry("Notify", "NotifySound"));

	soundfiles.clear();
	if (cb_soundtheme->currentText() == tr("Custom"))
		CONST_FOREACH(name, soundNames)
			soundfiles[*name] = config_file.readEntry("Sounds", (*name)+"_sound");
	else
	{
		QPushButton *choose = ConfigDialog::getPushButton("Sounds","Choose");
		QPushButton *clear = ConfigDialog::getPushButton("Sounds","Clear");
		choose->setEnabled(false);
		clear->setEnabled(false);

		CONST_FOREACH(name, soundNames)
			soundfiles[*name] = sound_manager->theme()->themePath()+sound_manager->theme()->getThemeEntry(*name);
	}

	QStringList::const_iterator text = soundTexts.begin();
	CONST_FOREACH(name, soundNames)
		new QListViewItem(lv_soundfiles, *text++, soundfiles[*name]);

	QVBox* util_box=ConfigDialog::getVBox("Sounds","util_box");
	util_box->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

	soundPlayer(b_playsound->isChecked());

	g_volume->setEnabled(b_playsound->isChecked() && b_volumectrl->isChecked());

	connect(b_volumectrl, SIGNAL(toggled(bool)), g_volume, SLOT(setEnabled(bool)));

	SelectPaths *selpaths = ConfigDialog::getSelectPaths("Sounds", "Sound paths");
	selpaths->setPathList(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	kdebugf2();
}

void SoundSlots::muteActionActivated(const UserGroup* /*users*/, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	sound_manager->setMute(is_on);
	mute_action->setAllOn(is_on);
	config_file.writeEntry("Sounds", "PlaySound", !is_on);

	if (ConfigDialog::dialogOpened())
	{
		QCheckBox *box=ConfigDialog::getCheckBox("Sounds", "Play sounds");
		if (box->isChecked()==is_on)
		{
			box->setChecked(!is_on);
			soundPlayer(!is_on, true);
		}
	}
	kdebugf2();
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	muteActionActivated(NULL, NULL, !sound_manager->isMuted());
	kdebugf2();
}


void SoundSlots::soundPlayer(bool value, bool toolbarChanged)
{
	kdebugf();
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");

	ConfigDialog::getHBox("Sounds","sound_box")->setEnabled(value);
	ConfigDialog::getHBox("Sounds","sound_theme")->setEnabled(value);

	b_volumectrl->setEnabled(value);
	ConfigDialog::getGrid("Sounds","volume")->setEnabled(value && b_volumectrl->isChecked());
	if (value==sound_manager->isMuted() && !toolbarChanged)
		muteUnmuteSounds();
	kdebugf2();
}

void SoundSlots::clearSoundFile()
{
	kdebugf();
	QListViewItem *item=ConfigDialog::getListView("Sounds", "sound_files")->currentItem();
	if (!item->isSelected())
		return;
	item->setText(1, QString::null);
	kdebugf2();
}

void SoundSlots::chooseSoundFile()
{
	kdebugf();
	QString start=QDir::rootDirPath();
	QListViewItem *item=ConfigDialog::getListView("Sounds", "sound_files")->currentItem();
	if (!item->isSelected())
		return;

	QString p=item->text(1);
	if (QFile(p).exists())
		start=p;

	QString s(QFileDialog::getOpenFileName( start, "Audio Files (*.wav *.au *.raw)", ConfigDialog::configdialog));
	if (!s.isEmpty())
		item->setText(1,s);
	kdebugf2();
}


void SoundSlots::testSoundFile()
{
	kdebugf();
	QListViewItem *item=ConfigDialog::getListView("Sounds", "sound_files")->currentItem();
	if (!item->isSelected())
		return;
	sound_manager->play(item->text(1), true);
	kdebugf2();
}

void SoundSlots::chooseSoundTheme(const QString& string)
{
	kdebugf();
	QString str=string;
	if (string == tr("Custom"))
		str= "Custom";
	else if (string == tr("default"))
		str= "default";
	sound_manager->theme()->setTheme(str);

	QPushButton *choose = ConfigDialog::getPushButton("Sounds","Choose");
	QPushButton *clear = ConfigDialog::getPushButton("Sounds","Clear");

	QString chatfile;
	QString messagefile;
	QString notifyfile;

	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	lv_soundfiles->clear();
	soundfiles.clear();
	if (str == "Custom")
	{
		CONST_FOREACH(name, soundNames)
			soundfiles[*name] = config_file.readEntry("Sounds", (*name)+"_sound");
		choose->setEnabled(true);
		clear->setEnabled(true);
	}
	else
	{
		CONST_FOREACH(name, soundNames)
			soundfiles[*name] = sound_manager->theme()->themePath()+sound_manager->theme()->getThemeEntry(*name);
		choose->setEnabled(false);
		clear->setEnabled(false);
	}

	QStringList::const_iterator text = soundTexts.begin();
	CONST_FOREACH(name, soundNames)
		new QListViewItem(lv_soundfiles, *text++, soundfiles[*name]);
	kdebugf2();
}


void SoundSlots::selectedPaths(const QStringList& paths)
{
	kdebugf();
	sound_manager->theme()->setPaths(paths);
	QComboBox* cb_soundtheme= ConfigDialog::getComboBox("Sounds","Sound theme");
	QString current= cb_soundtheme->currentText();

	SelectPaths* soundPath = ConfigDialog::getSelectPaths("Sounds","Sound paths");
	soundPath->setPathList(sound_manager->theme()->additionalPaths());

	cb_soundtheme->clear();
	cb_soundtheme->insertItem("Custom");// 0 position
	cb_soundtheme->insertStringList(sound_manager->theme()->themes());
	cb_soundtheme->setCurrentText(current);
	cb_soundtheme->changeItem(tr("Custom"), 0);// and now add translation

	if (paths.contains("default"))
		cb_soundtheme->changeItem(tr("default"), paths.findIndex("default")+1);
	kdebugf2();
}

void SoundSlots::onApplyTabSounds()
{
	kdebugf();
	QComboBox *cb_soundtheme= ConfigDialog::getComboBox("Sounds", "Sound theme");
	QString theme;
	if (cb_soundtheme->currentText() == tr("Custom"))
	{
		QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
		theme= "Custom";

		QStringList::const_iterator text = soundTexts.begin();
		CONST_FOREACH(name, soundNames)
			config_file.writeEntry("Sounds", (*name)+"_sound", lv_soundfiles->findItem(*text++, 0)->text(1));
	}
	else
		theme= cb_soundtheme->currentText();
	if (theme == tr("default"))
		theme= "default";

	config_file.writeEntry("Sounds", "SoundPaths", sound_manager->theme()->additionalPaths().join(";"));
	config_file.writeEntry("Sounds", "SoundTheme", theme);
	kdebugf2();
}

void SoundSlots::testSamplePlaying()
{
	kdebugf();
	if (SamplePlayingTestMsgBox != NULL)
		return;
	QString chatsound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		chatsound = config_file.readEntry("Sounds", "NewChat_sound");
	else
		chatsound = sound_manager->theme()->themePath(config_file.readEntry("Sounds", "SoundTheme")) + sound_manager->theme()->getThemeEntry("NewChat");

	QFile file(chatsound);
	if (!file.open(IO_ReadOnly))
	{
		MessageBox::wrn(tr("Opening test sample file failed."));
		return;
	}
	// we are allocating 1 more word just in case of file.size() % sizeof(int16_t) != 0
	SamplePlayingTestSample = new int16_t[file.size() / sizeof(int16_t) + 1];
	if (file.readBlock((char*)SamplePlayingTestSample, file.size()) != (unsigned)file.size())
	{
		MessageBox::wrn(tr("Reading test sample file failed."));
		file.close();
		delete[] SamplePlayingTestSample;
		SamplePlayingTestSample = NULL;
		return;
	}
	file.close();

	SamplePlayingTestDevice = sound_manager->openDevice(PLAY_ONLY, 11025);
	if (SamplePlayingTestDevice == NULL)
	{
		MessageBox::wrn(tr("Opening sound device failed."));
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
		MessageBox::wrn(tr("Opening sound device failed."));
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
			MessageBox::wrn(tr("Cannot open sound device for playing!"));
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
		MessageBox::wrn(tr("Opening sound device failed."));
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

