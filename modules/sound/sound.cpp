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

extern "C" int sound_init()
{
	sound_manager=new SoundManager("sounds", "sound.conf");
	
	QObject::connect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		sound_manager, SLOT(chatSound(UinsList, const QString&, time_t,bool&)));
	QObject::connect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		sound_manager, SLOT(messageSound(UinsList, const QString&,time_t)));
	QObject::connect(&userlist, SIGNAL(changingStatus(const uin_t, const unsigned int, const unsigned int)),
		sound_manager, SLOT(notifySound(const uin_t, const unsigned int, const unsigned int)));

	//potrzebne do translacji
	QT_TRANSLATE_NOOP("@default","Sounds");
	QT_TRANSLATE_NOOP("@default","Play sounds");
	QT_TRANSLATE_NOOP("@default","Play sounds using aRts! server");
	QT_TRANSLATE_NOOP("@default","Sound player");
	QT_TRANSLATE_NOOP("@default","Path:");
	QT_TRANSLATE_NOOP("@default","Enable volume control (player must support it)");
	QT_TRANSLATE_NOOP("@default","Message sound");
	QT_TRANSLATE_NOOP("@default","Test");
	QT_TRANSLATE_NOOP("@default","Play sounds from a person whilst chatting");
	QT_TRANSLATE_NOOP("@default","Play chat sounds only when window is invisible");
	QT_TRANSLATE_NOOP("@default","Chat sound");
	QT_TRANSLATE_NOOP("@default","Notify sound");
	QT_TRANSLATE_NOOP("@default","Volume");
	QT_TRANSLATE_NOOP("@default","Choose");
	QT_TRANSLATE_NOOP("@default","Clear");
	QT_TRANSLATE_NOOP("@default","Test");
	QT_TRANSLATE_NOOP("@default","Sound paths");
	QT_TRANSLATE_NOOP("@default","Sound theme");
	QT_TRANSLATE_NOOP("@default", "Notify");
	QT_TRANSLATE_NOOP("@default", "Notify options");
	QT_TRANSLATE_NOOP("@default", "Notify by sound");

	ConfigDialog::addTab("Notify");
	ConfigDialog::addVGroupBox("Notify", "Notify", "Notify options");
	ConfigDialog::addCheckBox("Notify", "Notify options", "Notify by sound", "NotifyWithSound", false);
	
	
	ConfigDialog::addTab("Sounds");
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds", "PlaySound", false);
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds using aRts! server", "PlaySoundArtsDsp", false);
	
	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Sound player");
	ConfigDialog::addLineEdit("Sounds", "Sound player", "Path:", "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "OpenFile","","soundplayer_fileopen");
	
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Enable volume control (player must support it)", "VolumeControl", false);
	ConfigDialog::addGrid("Sounds", "Sounds", "volume", 2);
	ConfigDialog::addLabel("Sounds", "volume", "Volume");
	ConfigDialog::addSlider("Sounds", "volume", "slider", "SoundVolume", 0, 400, 50, 200);
	
	ConfigDialog::addHBox("Sounds", "Sounds", "sound_theme");
	ConfigDialog::addComboBox("Sounds", "sound_theme", "Sound theme");
	ConfigDialog::addSelectPaths("Sounds", "sound_theme", "Sound paths");

	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds from a person whilst chatting", "PlaySoundChat", true);
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play chat sounds only when window is invisible", "PlaySoundChatInvisible", true);
	
	ConfigDialog::addHBox("Sounds", "Sounds", "sound_box");
	ConfigDialog::addListView("Sounds", "sound_box", "sound_files");
	ConfigDialog::addVBox("Sounds", "sound_box", "util_box");
	ConfigDialog::addPushButton("Sounds", "util_box", "Choose");
	ConfigDialog::addPushButton("Sounds", "util_box", "Clear");
	ConfigDialog::addPushButton("Sounds", "util_box", "Test");

	soundslots= new SoundSlots();

	ConfigDialog::registerSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(soundslots, SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), soundslots, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "Choose", SIGNAL(released()), soundslots, SLOT(chooseSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Clear", SIGNAL(released()), soundslots, SLOT(clearSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(released()), soundslots, SLOT(testSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), soundslots, SLOT(chooseSoundTheme(const QString&)));
	ConfigDialog::connectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), soundslots, SLOT(selectedPaths(const QStringList&)));
	
	config_file.addVariable("Sounds", "SoundTheme", "default");
	
	config_file.addVariable("Sounds", "SoundPaths",sound_manager->defaultKaduPathsWithThemes().join(";"));
	
	sound_manager->setPaths(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	sound_manager->setTheme(config_file.readEntry("Sounds","SoundTheme"));
		
	QObject::connect(sound_manager, SIGNAL(playFile(const QString&)), soundslots, SLOT(playingSound(const QString&)));

	return 0;
}

extern "C" void sound_close()
{
	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		sound_manager, SLOT(chatSound(UinsList, const QString&, time_t,bool&)));
	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		sound_manager, SLOT(messageSound(UinsList, const QString&,time_t)));
	QObject::disconnect(&userlist, SIGNAL(changingStatus(const uin_t, const unsigned int, const unsigned int)),
		sound_manager, SLOT(notifySound(const uin_t, const unsigned int, const unsigned int)));

	QObject::disconnect(sound_manager, SIGNAL(playFile(const QString&)), soundslots, SLOT(playingSound(const QString&)));

	delete sound_manager;
	delete soundslots;
}

SoundManager* sound_manager;

SoundManager::SoundManager(const QString& name, const QString& configname)
	:Themes(name, configname)
{
	mute = false;
	lastsoundtime.start();
};


void SoundManager::playSound(const QString &sound, const QString player) {
	if (!config_file.readBoolEntry("Sounds","PlaySound") || mute)
		return;

	QStringList args;
	if ((QString::compare(sound, NULL) == 0) || (QString::compare(sound, "") == 0)) {
		kdebug("No sound file specified?\n");
		return;
		}
	if (config_file.readBoolEntry("Sounds","PlaySoundArtsDsp"))
		args.append("artsdsp");
	if (player == QString::null)
		args.append(config_file.readEntry("Sounds","SoundPlayer"));
	else
		args.append(player);
	if (config_file.readBoolEntry("Sounds","VolumeControl"))
		args.append(QString("-v %1").arg(config_file.readDoubleNumEntry("Sounds","SoundVolume")/100));
	args.append(sound);
	for (QStringList::Iterator it = args.begin(); it != args.end(); ++it ) {
       		kdebug("playSound(): %s\n", (const char *)(*it).local8Bit());
		}
	QProcess *sndprocess = new QProcess(args);
	sndprocess->start();
	delete sndprocess;
	lastsoundtime.restart();
}

bool SoundManager::getMute()
{
    return mute;
}

void SoundManager::setMute(const bool& enable)
{
    mute= enable;
}

void SoundManager::messageSound(UinsList senders,const QString& msg,time_t time)
{
    emit playOnMessage(senders);

	UserListElement ule = userlist.byUinValue(senders[0]);
	QString messagesound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		messagesound=parse(config_file.readEntry("Sounds","Message_sound"),ule);
	else 
		messagesound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Message");
	emit playFile(messagesound);
}



void SoundManager::chatSound(UinsList senders,const QString& msg,time_t time, bool& grab)
{
		Chat* chat= chat_manager->findChatByUins(senders);
	if (config_file.readBoolEntry("Sounds","PlaySoundChat") && grab)
		{
		    if (config_file.readBoolEntry("Sounds","PlaySoundChatInvisible"))
			if (chat->isActiveWindow())
				return;
		emit playOnChat(senders);

	QString chatsound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		chatsound=config_file.readEntry("Sounds", "Chat_sound");
	else 
		chatsound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Chat");
	emit playFile(chatsound);

		}
}

void SoundManager::notifySound(const uin_t uin, const unsigned int oldstatus, const unsigned int status)
{
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
				emit playOnNotify(uin);
			
				QString notifysound;
				if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
					notifysound=parse(config_file.readEntry("Notify","NotifySound"),userlist.byUin(uin),false);
				else 
					notifysound=themePath(config_file.readEntry("Sounds", "SoundTheme"))+getThemeEntry("Notify");

			    	emit playFile(notifysound);
			}
}



int SoundManager::timeAfterLastSound()
{
    return lastsoundtime.elapsed();

}

SoundSlots* soundslots;

SoundSlots::SoundSlots()
{
	QIconSet *mu;
	if (sound_manager->getMute()) {
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Mute"), tr("Unmute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		mu= new QIconSet(icons_manager.loadIcon("Mute"));
		}
	else {
		muteitem= kadu->mainMenu()->insertItem(icons_manager.loadIcon("Unmute"), tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, -1, 3);
		mu= new QIconSet(icons_manager.loadIcon("Unmute"));
		}

	Kadu::addToolButton(*mu, tr("Mute sounds"), this, SLOT(muteUnmuteSounds()), 0, "mute");
}


void SoundSlots::onCreateConfigDialog()
{
	kdebug("SoundSlots::onCreateConfigDialog()\n");
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
	lv_soundfiles->setResizeMode(QListView::AllColumns);
	lv_soundfiles->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));
	    
	QString chatfile, messagefile, notifyfile;
	if (cb_soundtheme->currentText() == tr("Custom"))
	    {
	    chatfile= config_file.readEntry("Sounds", "Chat_sound");
	    messagefile= config_file.readEntry("Sounds", "Message_sound");
	    notifyfile= config_file.readEntry("Notify", "NotifySound");
	    }
	else 
	    {
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

	
};

void SoundSlots::playingSound(const QString& file)
{
	sound_manager->playSound(file);
}	

void SoundSlots::muteUnmuteSounds()
{
	QToolButton *mutebtn= Kadu::getToolButton("mute");
	sound_manager->setMute(!sound_manager->getMute());
	if (sound_manager->getMute()) {
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


void SoundSlots::soundPlayer(bool value)
{

	QCheckBox *b_playarts= ConfigDialog::getCheckBox("Sounds", "Play sounds using aRts! server");
	QHGroupBox *g_soundplayer= ConfigDialog::getHGroupBox("Sounds", "Sound player");
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

	    b_playarts->setEnabled(value);
	    g_soundplayer->setEnabled(value);
	    b_volumectrl->setEnabled(value);
	    g_volume->setEnabled(value && b_volumectrl->isChecked());
	    b_playchatting->setEnabled(value);
	    b_playinvisible->setEnabled(value && b_playchatting->isChecked());

}

void SoundSlots::choosePlayerFile()
{
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");

	QString s(QFileDialog::getOpenFileName( QString::null, "All Files (*)"));
	    if (s.length())
		    e_soundprog->setText(s);		
};

void SoundSlots::clearSoundFile()
{
kdebug("SoundSlots::clearSoundFile()\n");

    QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
    QListViewItem *item= lv_soundfiles->currentItem();
    if (!item->isSelected()) return;    
    item->setText(1, "");
};

void SoundSlots::chooseSoundFile()
{
kdebug("SoundSlots::chooseSoundFile()\n");
    QDir dir;
    QString startdir="/";
    QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
    QListViewItem *item= lv_soundfiles->currentItem();
    if (!item->isSelected()) return;

    if (dir.cd(item->text(1)) && (item->text(1)!= ""))
	    startdir=item->text(1);

	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)"));
	    if (s.length())
		    item->setText(1,s);
};


void SoundSlots::testSoundFile()
{
kdebug("SoundSlots::testSoundFile()\n");
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");
	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	QListViewItem *item= lv_soundfiles->currentItem();
	if (!item->isSelected()) return;    
	sound_manager->playSound(item->text(1), e_soundprog->text());
};

void SoundSlots::chooseSoundTheme(const QString& string)
{
	kdebug("SoundSlots::chooseSoundTheme()\n");
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
	    if (str == "Custom")
		{
		 chatfile=config_file.readEntry("Sounds", "Chat_sound");
	         messagefile=config_file.readEntry("Sounds", "Message_sound");
		 notifyfile= config_file.readEntry("Notify", "NotifySound");
		 choose->setEnabled(true);
		 clear->setEnabled(true);
		}
	    else 
		{

		 chatfile= sound_manager->themePath()+sound_manager->getThemeEntry("Chat");
	         messagefile= sound_manager->themePath()+sound_manager->getThemeEntry("Message");
	         notifyfile= sound_manager->themePath()+sound_manager->getThemeEntry("Notify");
		 choose->setEnabled(false);
		 clear->setEnabled(false);
		}

	    new QListViewItem(lv_soundfiles, tr("Chat sound"), chatfile);
	    new QListViewItem(lv_soundfiles, tr("Message sound"), messagefile);
	    new QListViewItem(lv_soundfiles, tr("Notify sound"), notifyfile);
};

void SoundSlots::selectedPaths(const QStringList& paths)
{
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
	sound_manager->setPaths(paths);

};

void SoundSlots::onDestroyConfigDialog()
{
	kdebug("SoundSlots::onDestroyConfigDialog()\n");
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
};
