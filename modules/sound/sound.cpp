/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qprocess.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qdir.h>
#include <qlayout.h>

#include "chat.h"
#include "sound.h"
#include "debug.h"
#include "config_file.h"
#include "config_dialog.h"
#include "kadu.h"
#include "../notify/notify.h"

SoundManager* sound_manager=NULL;
SoundSlots* soundslots;

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

SoundManager::SoundManager(const QString& name, const QString& configname)
	:Themes(name, configname, "sound_manager")
{
	mute = false;
	lastsoundtime.start();

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default","Sounds"), "SoundsTab");
	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Play sounds"), "PlaySound", false);
	
	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Enable volume control (player must support it)"), 
			"VolumeControl", true);
	ConfigDialog::addGrid("Sounds", "Sounds", "volume", 2);
	ConfigDialog::addLabel("Sounds", "volume", QT_TRANSLATE_NOOP("@default","Volume"));
	ConfigDialog::addSlider("Sounds", "volume", "slider", "SoundVolume", 0, 100, 20, 50);
	
	ConfigDialog::addHBox("Sounds", "Sounds", "sound_theme");
	ConfigDialog::addComboBox("Sounds", "sound_theme",
			QT_TRANSLATE_NOOP("@default","Sound theme"));
	ConfigDialog::addSelectPaths("Sounds", "sound_theme",
			QT_TRANSLATE_NOOP("@default","Sound paths"));

	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Play sounds from a person whilst chatting"),
			"PlaySoundChat", true);
	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Play chat sounds only when window is invisible"),
			"PlaySoundChatInvisible", true);
	
	ConfigDialog::addHBox("Sounds", "Sounds", "sound_box");
	ConfigDialog::addListView("Sounds", "sound_box", "sound_files");
	ConfigDialog::addVBox("Sounds", "sound_box", "util_box");
	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Choose"));
	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Clear"));
	ConfigDialog::addPushButton("Sounds", "util_box", QT_TRANSLATE_NOOP("@default","Test"));
	
	sound_manager=this;
	soundslots= new SoundSlots();

	ConfigDialog::registerSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(soundslots, SLOT(onApplyConfigDialog()));
	ConfigDialog::connectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::connectSlot("Sounds", "Choose", SIGNAL(clicked()), soundslots, SLOT(chooseSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Clear", SIGNAL(clicked()), soundslots, SLOT(clearSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(clicked()), soundslots, SLOT(testSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), soundslots, SLOT(chooseSoundTheme(const QString&)));
	ConfigDialog::connectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), soundslots, SLOT(selectedPaths(const QStringList&)));
	
	config_file.addVariable("Sounds", "SoundTheme", "default");
	config_file.addVariable("Sounds", "SoundPaths","");
	
	setPaths(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	setTheme(config_file.readEntry("Sounds","SoundTheme"));

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(const UinsList &, const QString &, time_t));
	s["NewMessage"]=SLOT(newMessage(const UinsList &, const QString &, time_t, bool &));
	s["ConnError"]=SLOT(connectionError(const QString &));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const UserListElement &));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const UserListElement &));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const UserListElement &));
	s["Message"]=SLOT(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *));
	
	config_file.addVariable("Notify", "NewChat_Sound", true);
	config_file.addVariable("Notify", "NewMessage_Sound", true);
	config_file.addVariable("Notify", "ConnError_Sound", true);
	config_file.addVariable("Notify", "toAvailable_Sound", true);
	config_file.addVariable("Notify", "toBusy_Sound", true);
	config_file.addVariable("Notify", "toNotAvailable_Sound", false);
	config_file.addVariable("Notify", "Message_Sound", true);

	notify->registerNotifier("Sound", this, s);
}

SoundManager::~SoundManager()
{
	ConfigDialog::unregisterSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(soundslots, SLOT(onApplyConfigDialog()));
	ConfigDialog::disconnectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::disconnectSlot("Sounds", "Choose", SIGNAL(clicked()), soundslots, SLOT(chooseSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Clear", SIGNAL(clicked()), soundslots, SLOT(clearSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Test", SIGNAL(clicked()), soundslots, SLOT(testSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), soundslots, SLOT(chooseSoundTheme(const QString&)));
	ConfigDialog::disconnectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), soundslots, SLOT(selectedPaths(const QStringList&)));

	delete soundslots;
	soundslots=NULL;

	ConfigDialog::removeControl("Sounds", "Test");
	ConfigDialog::removeControl("Sounds", "Clear");
	ConfigDialog::removeControl("Sounds", "Choose");
	ConfigDialog::removeControl("Sounds", "util_box");
	ConfigDialog::removeControl("Sounds", "sound_files");
	ConfigDialog::removeControl("Sounds", "sound_box");
	ConfigDialog::removeControl("Sounds", "Play chat sounds only when window is invisible");
	ConfigDialog::removeControl("Sounds", "Play sounds from a person whilst chatting");
	ConfigDialog::removeControl("Sounds", "Sound paths");
	ConfigDialog::removeControl("Sounds", "Sound theme");
	ConfigDialog::removeControl("Sounds", "sound_theme");
	ConfigDialog::removeControl("Sounds", "slider");
	ConfigDialog::removeControl("Sounds", "Volume");
	ConfigDialog::removeControl("Sounds", "volume");
	ConfigDialog::removeControl("Sounds", "Enable volume control (player must support it)");
	ConfigDialog::removeControl("Sounds", "Play sounds");
	ConfigDialog::removeTab("Sounds");

	notify->unregisterNotifier("Sound");
}

bool SoundManager::isMuted()
{
	return mute;
}

void SoundManager::setMute(const bool& enable)
{
	mute= enable;
}

void SoundManager::newChat(const UinsList &senders, const QString& msg, time_t time)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::newChat() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::newChat() end: too often, exiting\n");
		return;
	}

	if (config_file.readBoolEntry("Sounds","PlaySoundChat"))
	{
		QString chatsound;
		if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
			chatsound=config_file.readEntry("Sounds", "Chat_sound");
		else 
			chatsound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Chat");
		if (QFile::exists(chatsound))
			emit playSound(chatsound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
		lastsoundtime.restart();
	}
	kdebugf2();
}

void SoundManager::newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::newMessage() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::newMessage() end: too often, exiting\n");
		return;
	}

	Chat* chat= chat_manager->findChatByUins(senders);
	if (config_file.readBoolEntry("Sounds","PlaySoundChatInvisible") && chat->isActiveWindow())
		return;
	
	UserListElement ule = userlist.byUinValue(senders[0]);
	QString messagesound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		messagesound=parse(config_file.readEntry("Sounds","Message_sound"),ule);
	else 
		messagesound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Message");
	if (QFile::exists(messagesound))
		emit playSound(messagesound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::connectionError(const QString &message)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::connectionError() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::connectionError() end: too often, exiting\n");
		return;
	}

	QString conn_error_sound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		conn_error_sound=config_file.readEntry("Sounds","ConnectionError_sound");
	else 
		conn_error_sound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("ConnectionError");
	if (QFile::exists(conn_error_sound))
		emit playSound(conn_error_sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::userChangedStatusToAvailable(const UserListElement &ule)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToAvailable() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToAvailable() end: too often, exiting\n");
		return;
	}

	QString status_change_sound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		status_change_sound=parse(config_file.readEntry("Sounds","StatusAvailable_sound"), ule);
	else 
		status_change_sound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("StatusAvailable");
	if (QFile::exists(status_change_sound))
		emit playSound(status_change_sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::userChangedStatusToBusy(const UserListElement &ule)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToBusy() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToBusy() end: too often, exiting\n");
		return;
	}

	QString status_change_sound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		status_change_sound=parse(config_file.readEntry("Sounds","StatusBusy_sound"), ule);
	else 
		status_change_sound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("StatusBusy");
	if (QFile::exists(status_change_sound))
		emit playSound(status_change_sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::userChangedStatusToNotAvailable(const UserListElement &ule)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToNotAvailable() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::userChangedStatusToNotAvailable() end: too often, exiting\n");
		return;
	}
	
	QString status_change_sound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		status_change_sound=parse(config_file.readEntry("Sounds","StatusNotAvailable_sound"), ule);
	else 
		status_change_sound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("StatusNotAvailable");
	if (QFile::exists(status_change_sound))
		emit playSound(status_change_sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::message(const QString &, const QString &message, const QMap<QString, QVariant> *, const UserListElement *)
{
	kdebugf();
	if (isMuted())
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::message() end: muted\n");
		return;
	}
	if (timeAfterLastSound()<500)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::message() end: too often, exiting\n");
		return;
	}

	QString message_sound;
	if (message!=QString::null)
		message_sound=message;
	else if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		message_sound=config_file.readEntry("Sounds","OtherMessage_sound");
	else 
		message_sound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("OtherMessage");
	if (QFile::exists(message_sound))
		emit playSound(message_sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
	kdebugf2();
}

void SoundManager::play(const QString &path, bool force)
{
	kdebugf();
	bool volCntrl;
	float vol;
	
	if (isMuted() && !force)
	{
		kdebugm(KDEBUG_FUNCTION_END, "SoundManager::play() end: muted\n");
		return;
	}
	if (ConfigDialog::dialogOpened())
	{
		volCntrl=ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)")->isChecked();
		vol=1.0*ConfigDialog::getSlider("Sounds", "slider")->value()/100;
	}
	else
	{
		volCntrl=config_file.readBoolEntry("Sounds","VolumeControl");
		vol=1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100;
	}
	if (QFile::exists(path))
		emit playSound(path, volCntrl, vol);
	kdebugf2();
}

int SoundManager::timeAfterLastSound()
{
	return lastsoundtime.elapsed();
}

SoundSlots::SoundSlots() : QObject(NULL, "sound_slots")
{
	kdebugf();

	soundNames<<"Chat"<<"Message"<<"StatusAvailable"<<"StatusBusy"<<
			"StatusNotAvailable"<<"ConnectionError"<<"OtherMessage";
	
	soundTexts<<tr("Chat sound")<<tr("Message sound")<<tr("Status available sound")<<
				tr("Status busy sound")<<tr("Status not available sound")<<
				tr("Conection error sound")<<tr("Other message");

	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));
	if (sound_manager->isMuted())
	{
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Mute"), tr("Unmute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		ToolBar::registerButton("Mute", tr("Unmute sounds"), this, SLOT(muteUnmuteSounds()), 0, "mute");
	}
	else
	{
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Unmute"), tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		ToolBar::registerButton("Unmute", tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, "mute");
	}

	kdebugf2();
}

SoundSlots::~SoundSlots()
{
	kdebugf();
	kadu->mainMenu()->removeItem(muteitem);
	ToolBar::unregisterButton("mute");
	kdebugf2();
}

void SoundSlots::onCreateConfigDialog()
{
	kdebugf();
	QCheckBox *b_playsound= ConfigDialog::getCheckBox("Sounds", "Play sounds");
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QGrid *g_volume= ConfigDialog::getGrid("Sounds","volume");
	QCheckBox *b_playchatting= ConfigDialog::getCheckBox("Sounds", "Play sounds from a person whilst chatting");
	QCheckBox *b_playinvisible= ConfigDialog::getCheckBox("Sounds", "Play chat sounds only when window is invisible");
	QComboBox *cb_soundtheme= ConfigDialog::getComboBox("Sounds", "Sound theme");
	cb_soundtheme->insertItem("Custom");// 0-wa pozycja
	cb_soundtheme->insertStringList(sound_manager->themes());
	cb_soundtheme->setCurrentText(config_file.readEntry("Sounds", "SoundTheme"));
	cb_soundtheme->changeItem(tr("Custom"), 0);// dodanie translacji 
	if (sound_manager->themes().contains("default"))
		cb_soundtheme->changeItem(tr("default"), sound_manager->themes().findIndex("default")+1);

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
		for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it)
			soundfiles[*it]=config_file.readEntry("Sounds", (*it)+"_sound");
	else
	{
		QPushButton *choose = ConfigDialog::getPushButton("Sounds","Choose");
		QPushButton *clear = ConfigDialog::getPushButton("Sounds","Clear");
		choose->setEnabled(false);
		clear->setEnabled(false);

		for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it)
			soundfiles[*it]=sound_manager->themePath()+sound_manager->getThemeEntry(*it);
	}
	
	QStringList::iterator it2=soundTexts.begin();
	for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it, ++it2)
		new QListViewItem(lv_soundfiles, *it2, soundfiles[*it]);

	QVBox* util_box=ConfigDialog::getVBox("Sounds","util_box");
	util_box->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	
	soundPlayer(b_playsound->isChecked());
	
	g_volume->setEnabled(b_playsound->isChecked() && b_volumectrl->isChecked());
	b_playinvisible->setEnabled(b_playsound->isChecked()&& b_playchatting->isChecked());

	connect(b_volumectrl,SIGNAL(toggled(bool)), g_volume, SLOT(setEnabled(bool)));
	connect(b_playchatting,SIGNAL(toggled(bool)), b_playinvisible, SLOT(setEnabled(bool)));

	SelectPaths *selpaths= ConfigDialog::getSelectPaths("Sounds", "Sound paths");
	QStringList pl(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	selpaths->setPathList(pl);
	kdebugf2();
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	bool mute=!sound_manager->isMuted();
	sound_manager->setMute(mute);
	config_file.writeEntry("Sounds", "PlaySound", !mute);
	
	if (ConfigDialog::dialogOpened())
	{
		QCheckBox *box=ConfigDialog::getCheckBox("Sounds", "Play sounds");
		if (box->isChecked()==mute)
		{
			box->setChecked(!mute);
			soundPlayer(!mute, true);
		}
	}

	if (mute)
	{
		ToolBar::refreshIcons(tr("Mute sounds"), "Mute", tr("Unmute sounds"));
		kadu->menuBar()->changeItem(muteitem, icons_manager.loadIcon("Mute"), tr("Unmute sounds"));
	}
	else
	{
		ToolBar::refreshIcons(tr("Unmute sounds"), "Unmute", tr("Mute sounds"));
		kadu->menuBar()->changeItem(muteitem, icons_manager.loadIcon("Unmute"), tr("Mute sounds"));
	}

	kdebugf2();
}


void SoundSlots::soundPlayer(bool value, bool toolbarChanged)
{
	kdebugf();
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QCheckBox *b_playchatting= ConfigDialog::getCheckBox("Sounds", "Play sounds from a person whilst chatting");
	QCheckBox *b_playinvisible= ConfigDialog::getCheckBox("Sounds", "Play chat sounds only when window is invisible");

	ConfigDialog::getHBox("Sounds","sound_box")->setEnabled(value);
	ConfigDialog::getHBox("Sounds","sound_theme")->setEnabled(value);

	b_volumectrl->setEnabled(value);
	ConfigDialog::getGrid("Sounds","volume")->setEnabled(value && b_volumectrl->isChecked());
	b_playchatting->setEnabled(value);
	b_playinvisible->setEnabled(value && b_playchatting->isChecked());
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
	item->setText(1, "");
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
	
	QString s(QFileDialog::getOpenFileName( start, "Audio Files (*.wav *.au *.raw)"));
	if (s.length())
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
	sound_manager->setTheme(str);

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
		for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it)
			soundfiles[*it]=config_file.readEntry("Sounds", (*it)+"_sound");
		choose->setEnabled(true);
		clear->setEnabled(true);
	}
	else
	{
		for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it)
			soundfiles[*it]=sound_manager->themePath()+sound_manager->getThemeEntry(*it);
		choose->setEnabled(false);
		clear->setEnabled(false);
	}

	QStringList::iterator it2=soundTexts.begin();
	for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it, ++it2)
		new QListViewItem(lv_soundfiles, *it2, soundfiles[*it]);
	kdebugf2();
}


void SoundSlots::selectedPaths(const QStringList& paths)
{
	kdebugf();
	sound_manager->setPaths(paths);
	QComboBox* cb_soundtheme= ConfigDialog::getComboBox("Sounds","Sound theme");
	QString current= cb_soundtheme->currentText();
	
	SelectPaths* soundPath = ConfigDialog::getSelectPaths("Sounds","Sound paths");
	soundPath->setPathList(sound_manager->additionalPaths());
	
	cb_soundtheme->clear();
	cb_soundtheme->insertItem("Custom");// 0-wa pozycja
	cb_soundtheme->insertStringList(sound_manager->themes());
	cb_soundtheme->setCurrentText(current);
	cb_soundtheme->changeItem(tr("Custom"), 0);// dodanie translacji 

	if (paths.contains("default"))
		cb_soundtheme->changeItem(tr("default"), paths.findIndex("default")+1);
	kdebugf2();
}

void SoundSlots::onApplyConfigDialog()
{
	kdebugf();
	QComboBox *cb_soundtheme= ConfigDialog::getComboBox("Sounds", "Sound theme");
	QString theme;
	if (cb_soundtheme->currentText() == tr("Custom"))
	{
		QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
		theme= "Custom";

		QStringList::iterator it2=soundTexts.begin();
		for (QStringList::iterator it=soundNames.begin(); it!=soundNames.end(); ++it, ++it2)
			config_file.writeEntry("Sounds", (*it)+"_sound", lv_soundfiles->findItem(*it2, 0)->text(1));
	}
	else
		theme= cb_soundtheme->currentText();
	if (theme == tr("default"))
		theme= "default";

	config_file.writeEntry("Sounds", "SoundPaths", sound_manager->additionalPaths().join(";"));
	config_file.writeEntry("Sounds", "SoundTheme", theme);
	kdebugf2();
}
