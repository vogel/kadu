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
#include "events.h"
#include "sound.h"
#include "debug.h"
#include "config_file.h"
#include "config_dialog.h"
#include "kadu.h"
#include "status.h"

SoundManager* sound_manager=NULL;
SoundSlots* soundslots;

extern "C" int sound_init()
{
	kdebugf();
	sound_manager=new SoundManager("sounds", "sound.conf");
	
	QObject::connect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		sound_manager, SLOT(chatSound(UinsList, const QString&, time_t,bool&)));
	QObject::connect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		sound_manager, SLOT(messageSound(UinsList, const QString&,time_t)));
	QObject::connect(&userlist, SIGNAL(changingStatus(const uin_t, const unsigned int, const unsigned int)),
		sound_manager, SLOT(notifySound(const uin_t, const unsigned int, const unsigned int)));

	//potrzebne do translacji
	
//	QT_TRANSLATE_NOOP("@default","Message sound");
//	QT_TRANSLATE_NOOP("@default","Notify sound");
						
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"));
	ConfigDialog::addVGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify options"));
	ConfigDialog::addCheckBox("Notify", "Notify options",
			QT_TRANSLATE_NOOP("@default", "Notify by sound"), "NotifyWithSound", false);	
	
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default","Sounds"));
	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Play sounds"), "PlaySound", false);
	
	ConfigDialog::addCheckBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Enable volume control (player must support it)"), 
			"VolumeControl", false);
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
	
	config_file.addVariable("Sounds", "SoundPaths",sound_manager->defaultKaduPathsWithThemes().join(";"));
	
	sound_manager->setPaths(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	sound_manager->setTheme(config_file.readEntry("Sounds","SoundTheme"));
		
	return 0;
}

extern "C" void sound_close()
{
	kdebugf();
	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		sound_manager, SLOT(chatSound(UinsList, const QString&, time_t,bool&)));
	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		sound_manager, SLOT(messageSound(UinsList, const QString&,time_t)));
	QObject::disconnect(&userlist, SIGNAL(changingStatus(const uin_t, const unsigned int, const unsigned int)),
		sound_manager, SLOT(notifySound(const uin_t, const unsigned int, const unsigned int)));

	ConfigDialog::unregisterSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(soundslots, SLOT(onApplyConfigDialog()));
	ConfigDialog::disconnectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::disconnectSlot("Sounds", "Choose", SIGNAL(clicked()), soundslots, SLOT(chooseSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Clear", SIGNAL(clicked()), soundslots, SLOT(clearSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Test", SIGNAL(clicked()), soundslots, SLOT(testSoundFile()));
	ConfigDialog::disconnectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), soundslots, SLOT(chooseSoundTheme(const QString&)));
	ConfigDialog::disconnectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), soundslots, SLOT(selectedPaths(const QStringList&)));

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
	ConfigDialog::removeControl("Notify", "Notify by sound");
	ConfigDialog::removeTab("Sounds");

	delete sound_manager;
	delete soundslots;
}


SoundManager::SoundManager(const QString& name, const QString& configname)
	:Themes(name, configname)
{
	mute = false;
	lastsoundtime.start();
}

bool SoundManager::isMuted()
{
	return mute;
}

void SoundManager::setMute(const bool& enable)
{
	mute= enable;
}

void SoundManager::messageSound(UinsList senders,const QString& msg,time_t time)
{
	kdebugf();
	if (isMuted())
		return;
	UserListElement ule = userlist.byUinValue(senders[0]);
	QString messagesound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		messagesound=parse(config_file.readEntry("Sounds","Message_sound"),ule);
	else 
		messagesound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Message");
	if (QFile::exists(messagesound))
		emit playOnMessage(senders, messagesound, msg, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
	lastsoundtime.restart();
}

void SoundManager::chatSound(UinsList senders,const QString& msg,time_t time, bool& grab)
{
	kdebugf();
	if (isMuted())
		return;
	Chat* chat= chat_manager->findChatByUins(senders);
	if (config_file.readBoolEntry("Sounds","PlaySoundChat") && grab)
	{
		if (config_file.readBoolEntry("Sounds","PlaySoundChatInvisible"))
			if (chat->isActiveWindow())
				return;

		QString chatsound;
		if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
			chatsound=config_file.readEntry("Sounds", "Chat_sound");
		else 
			chatsound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Chat");
		if (QFile::exists(chatsound))
			emit playOnChat(senders, chatsound, msg, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
		lastsoundtime.restart();
	}
}

void SoundManager::notifySound(const uin_t uin, const unsigned int oldstatus, const unsigned int status)
{
	kdebugf();
	if (isMuted())
		return;
	UserListElement &user = userlist.byUin(uin);

	if (!config_file.readBoolEntry("Notify","NotifyStatusChange"))
		return;

	if (userlist.containsUin(uin)) {
		if (!user.notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;
	}
	else
		if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;

	if (config_file.readBoolEntry("Notify","NotifyStatusChange") && (status == GG_STATUS_AVAIL ||
		status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR
		|| status == GG_STATUS_BLOCKED) &&
		(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
		oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2)) 
		if (config_file.readBoolEntry("Notify","NotifyWithSound"))
			if (timeAfterLastSound()>500)
			{
				QString notifysound;
				if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
					notifysound=parse(config_file.readEntry("Notify","NotifySound"),userlist.byUin(uin),false);
				else 
					notifysound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Notify");
				if (QFile::exists(notifysound))
					emit playOnNotify(uin, notifysound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0*config_file.readDoubleNumEntry("Sounds","SoundVolume")/100);
				lastsoundtime.restart();
			}
}

void SoundManager::play(const QString &path, bool force)
{
	kdebugf();
	bool volCntrl;
	float vol;
	
	if (isMuted() && !force)
		return;
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
}

int SoundManager::timeAfterLastSound()
{
	return lastsoundtime.elapsed();
}

SoundSlots::SoundSlots()
{
	kdebugf();
	QIconSet mu;
	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));
	if (sound_manager->isMuted()) {
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Mute"), tr("Unmute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		mu= QIconSet(icons_manager.loadIcon("Mute"));
	}
	else {
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Unmute"), tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		mu= QIconSet(icons_manager.loadIcon("Unmute"));
	}

	ToolBar::registerButton(mu, tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, "mute");
}

SoundSlots::~SoundSlots()
{
	kdebugf();
	kadu->mainMenu()->removeItem(muteitem);
	ToolBar::unregisterButton("mute");
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
	
	QString chatfile, messagefile, notifyfile;
	if (cb_soundtheme->currentText() == tr("Custom")) {
		chatfile= config_file.readEntry("Sounds", "Chat_sound");
		messagefile= config_file.readEntry("Sounds", "Message_sound");
		notifyfile= config_file.readEntry("Notify", "NotifySound");
	}
	else {
		QPushButton *choose = ConfigDialog::getPushButton("Sounds","Choose");
		QPushButton *clear = ConfigDialog::getPushButton("Sounds","Clear");
		choose->setEnabled(false);
		clear->setEnabled(false);
		chatfile= sound_manager->themePath()+sound_manager->getThemeEntry("Chat");
		messagefile= sound_manager->themePath()+sound_manager->getThemeEntry("Message");
		notifyfile= sound_manager->themePath()+sound_manager->getThemeEntry("Notify");
	}

	new QListViewItem(lv_soundfiles, tr("Chat sound"), chatfile);
	new QListViewItem(lv_soundfiles, tr("Message sound"), messagefile);
	new QListViewItem(lv_soundfiles, tr("Notify sound"), notifyfile);

	
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
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	QToolButton *mutebtn= ToolBar::getButton("mute");
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
	if (mute) {
		mutebtn->setIconSet(icons_manager.loadIcon("Mute"));
		mutebtn->setTextLabel(tr("Unmute sounds"));
		kadu->menuBar()->changeItem(muteitem, icons_manager.loadIcon("Mute"), tr("Unmute sounds"));
	}
	else {
		kadu->menuBar()->changeItem(muteitem, icons_manager.loadIcon("Unmute"), tr("Mute sounds"));
		mutebtn->setTextLabel(tr("Mute sounds"));
		mutebtn->setIconSet(icons_manager.loadIcon("Unmute"));
	}
}


void SoundSlots::soundPlayer(bool value, bool toolbarChanged)
{
	kdebugf();
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QGrid *g_volume= ConfigDialog::getGrid("Sounds","volume");
	QCheckBox *b_playchatting= ConfigDialog::getCheckBox("Sounds", "Play sounds from a person whilst chatting");
	QCheckBox *b_playinvisible= ConfigDialog::getCheckBox("Sounds", "Play chat sounds only when window is invisible");
	QCheckBox *b_notifysound= ConfigDialog::getCheckBox("Notify", "Notify by sound");

	QHBox* box=ConfigDialog::getHBox("Sounds","sound_box");
	box->setEnabled(value);
	QHBox* combobox=ConfigDialog::getHBox("Sounds","sound_theme");
	combobox->setEnabled(value);

	b_notifysound->setEnabled(value);

	b_volumectrl->setEnabled(value);
	g_volume->setEnabled(value && b_volumectrl->isChecked());
	b_playchatting->setEnabled(value);
	b_playinvisible->setEnabled(value && b_playchatting->isChecked());
	if (value==sound_manager->isMuted() && !toolbarChanged)
		muteUnmuteSounds();
}

void SoundSlots::clearSoundFile()
{
	kdebugf();
	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	QListViewItem *item= lv_soundfiles->currentItem();
	if (!item->isSelected())
		return;
	item->setText(1, "");
}

void SoundSlots::chooseSoundFile()
{
	kdebugf();
	QString start=QDir::rootDirPath();
	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	QListViewItem *item= lv_soundfiles->currentItem();
	if (!item->isSelected())
		return;

	QString p=item->text(1);
	if (QFile(p).exists())
		start=p;
	
	QString s(QFileDialog::getOpenFileName( start, "Audio Files (*.wav *.au *.raw)"));
	if (s.length())
		item->setText(1,s);
}


void SoundSlots::testSoundFile()
{
	kdebugf();
	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	QListViewItem *item= lv_soundfiles->currentItem();
	if (!item->isSelected())
		return;
	sound_manager->play(item->text(1), true);
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
	if (str == "Custom") {
		chatfile=config_file.readEntry("Sounds", "Chat_sound");
		messagefile=config_file.readEntry("Sounds", "Message_sound");
		notifyfile= config_file.readEntry("Notify", "NotifySound");
		choose->setEnabled(true);
		clear->setEnabled(true);
	}
	else {
		chatfile= sound_manager->themePath()+sound_manager->getThemeEntry("Chat");
		messagefile= sound_manager->themePath()+sound_manager->getThemeEntry("Message");
		notifyfile= sound_manager->themePath()+sound_manager->getThemeEntry("Notify");
		choose->setEnabled(false);
		clear->setEnabled(false);
	}

	new QListViewItem(lv_soundfiles, tr("Chat sound"), chatfile);
	new QListViewItem(lv_soundfiles, tr("Message sound"), messagefile);
	new QListViewItem(lv_soundfiles, tr("Notify sound"), notifyfile);
}

void SoundSlots::selectedPaths(const QStringList& paths)
{
	kdebugf();
	sound_manager->setPaths(paths);
	QComboBox* cb_soundtheme= ConfigDialog::getComboBox("Sounds","Sound theme");
	QString current= cb_soundtheme->currentText();
	cb_soundtheme->clear();
	cb_soundtheme->insertItem("Custom");// 0-wa pozycja
	cb_soundtheme->insertStringList(sound_manager->themes());
	cb_soundtheme->setCurrentText(current);
	cb_soundtheme->changeItem(tr("Custom"), 0);// dodanie translacji 

	if (paths.contains("default"))
		cb_soundtheme->changeItem(tr("default"), paths.findIndex("default")+1);
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
		config_file.writeEntry("Sounds", "Chat_sound", lv_soundfiles->findItem(tr("Chat sound"), 0)->text(1));
		config_file.writeEntry("Sounds", "Message_sound", lv_soundfiles->findItem(tr("Message sound"), 0)->text(1));
		config_file.writeEntry("Notify", "NotifySound", lv_soundfiles->findItem(tr("Notify sound"), 0)->text(1));
	}
	else
		theme= cb_soundtheme->currentText();
	if (theme == tr("default"))
		theme= "default";

	config_file.writeEntry("Sounds", "SoundPaths", sound_manager->paths().join(";"));
	config_file.writeEntry("Sounds", "SoundTheme", theme);
}
