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

#include "sound.h"
#include "debug.h"
#include "config_file.h"
#include "config_dialog.h"
#include "misc.h"


SoundManager::SoundManager()
{
	mute = false;
	lastsoundtime.start();
	ThemesList=getSubDirs(QString(DATADIR)+"/kadu/themes/sounds");
};

QStringList SoundManager::getSubDirs(const QString& path)
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList subdirs=dir.entryList();
	subdirs.remove(".");
	subdirs.remove("..");
	for (QStringList::Iterator it= subdirs.begin(); it!=subdirs.end(); it++)
		{
		QFile s(path+"/"+(*it)+"/sound.conf");
		if (!s.exists())
		    subdirs.remove((*it));
		}
	return subdirs;
};

const QStringList& SoundManager::themes()
{
	return ThemesList;
};

void SoundManager::setSoundTheme(const QString& theme)
{

	if(ThemesList.contains(theme))
	{
		entries.clear();
		config_file.writeEntry("Sounds","SoundTheme",theme);
		ConfigFile sound_theme_file(themePath()+fixFileName(themePath(),"sound.conf"));
		entries=sound_theme_file.getGroupSection("Sound");
	}
	else
		config_file.writeEntry("Sounds","SoundTheme","Custom");
	
	 
};


QString SoundManager::fixFileName(const QString& path,const QString& fn)
{
	// sprawd¼ czy oryginalna jest ok
	if(QFile::exists(path+"/"+fn))
		return fn;
	// mo¿e ca³o¶æ lowercase?
	if(QFile::exists(path+"/"+fn.lower()))
		return fn.lower();	
	// rozbij na nazwê i rozszerzenie
	QString name=fn.section('.',0,0);
	QString ext=fn.section('.',1);
	// mo¿e rozszerzenie uppercase?
	if(QFile::exists(path+"/"+name+"."+ext.upper()))
		return name+"."+ext.upper();
	// nie umiemy poprawiæ, zwracamy oryginaln±
	return fn;
};

void SoundManager::setSoundPaths(const QStringList& paths)
{
    QStringList add=paths;
	ThemesPaths=getSubDirs(QString(DATADIR)+"/kadu/themes/sounds");
	ThemesList=ThemesPaths;
	
	for (QStringList::Iterator it= ThemesPaths.begin(); it!=ThemesPaths.end(); it++)
		(*it)=QString(DATADIR)+"/kadu/themes/sounds/"+(*it)+"/";
		
	for (QStringList::Iterator it= add.begin(); it!=add.end(); it++)
		{
		QFile s((*it)+"/sound.conf");
		if (!s.exists())
		    add.remove((*it));
		else ThemesList.append((*it).section("/", -2));
		}
	ThemesPaths+=add;
};

QString SoundManager::themePath(const QString& theme)
{
    QString path;
    QString theme2=theme;
    if (theme == "")
	theme2=config_file.readEntry("Sounds", "SoundTheme");
    if (theme == "Custom")
	theme2="";

    	path=ThemesPaths.grep(theme2).first();
	return path;
};

QString SoundManager::getThemeEntry(const QString& name)
{
if (config_file.readEntry("Sounds", "SoundTheme") != "Custom")
    for (unsigned int i=0;i<entries.count();i++)
    {
        if (entries[i].name == name)
		return entries[i].value;
    }
return QString("");
}

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


int SoundManager::timeAfterLastSound()
{
    return lastsoundtime.elapsed();

}

void SoundManager::initModule()
{
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

	ConfigDialog::addTab("Sounds");
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds", "PlaySound", false);
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds using aRts! server", "PlaySoundArtsDsp", false);
	
	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Sound player");
	ConfigDialog::addLineEdit("Sounds", "Sound player", "Path:", "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "fileopen.png","","soundplayer_fileopen");
	
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

	
	SoundSlots *soundslots= new SoundSlots();
	ConfigDialog::registerSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(soundslots, SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), soundslots, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "Choose", SIGNAL(released()), soundslots, SLOT(chooseSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Clear", SIGNAL(released()), soundslots, SLOT(clearSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(released()), soundslots, SLOT(testSoundFile()));
	ConfigDialog::connectSlot("Sounds", "Sound theme", SIGNAL(activated(const QString&)), soundslots, SLOT(chooseSoundTheme(const QString&)));
	ConfigDialog::connectSlot("Sounds", "Sound paths", SIGNAL(changed(const QStringList&)), soundslots, SLOT(selectedPaths(const QStringList&)));
	
	config_file.addVariable("Sounds", "SoundTheme", "Custom");
	soundmanager.setSoundPaths(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	soundmanager.setSoundTheme(config_file.readEntry("Sounds","SoundTheme"));
};

SoundManager soundmanager;


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
	cb_soundtheme->insertStringList(soundmanager.themes());
	cb_soundtheme->setCurrentText(config_file.readEntry("Sounds", "SoundTheme"));
	cb_soundtheme->changeItem(tr("Custom"), 0);// dodanie translacji 
	cb_soundtheme->setEnabled(b_playsound->isChecked());

	QHBox* box=ConfigDialog::getHBox("Sounds","sound_box");
	box->setEnabled(b_playsound->isChecked());

	QComboBox* combobox=ConfigDialog::getComboBox("Sounds","Sound theme");
	combobox->setEnabled(b_playsound->isChecked());

	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds","sound_files");
	lv_soundfiles->addColumn(tr("Event"));
	lv_soundfiles->addColumn(tr("Sound file"));
	lv_soundfiles->setAllColumnsShowFocus(true);
	lv_soundfiles->setColumnWidthMode(0, QListView::Maximum);
	lv_soundfiles->setColumnWidthMode(1, QListView::Maximum);
	lv_soundfiles->setResizeMode(QListView::AllColumns);
	lv_soundfiles->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

	soundmanager.setSoundTheme(config_file.readEntry("Sounds","SoundTheme"));
	    
	QString chatfile, messagefile, notifyfile;
	if (combobox->currentText() == tr("Custom"))
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
	     chatfile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Chat");
	     messagefile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Message");
	     notifyfile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Notify");
	    }

	    new QListViewItem(lv_soundfiles, tr("Chat sound"), chatfile);
	    new QListViewItem(lv_soundfiles, tr("Message sound"), messagefile);
	    new QListViewItem(lv_soundfiles, tr("Notify sound"), notifyfile);

	
	QVBox* util_box=ConfigDialog::getVBox("Sounds","util_box");
	util_box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	
	soundPlayer(b_playsound->isChecked());
	
	g_volume->setEnabled(b_playsound->isChecked() && b_volumectrl->isChecked());
	b_playinvisible->setEnabled(b_playsound->isChecked()&& b_playchatting->isChecked());

	connect(b_volumectrl,SIGNAL(toggled(bool)), g_volume, SLOT(setEnabled(bool)));
	connect(b_playchatting,SIGNAL(toggled(bool)), b_playinvisible, SLOT(setEnabled(bool)));

	SelectPaths *selpaths= ConfigDialog::getSelectPaths("Sounds", "Sound paths");
	QStringList pl(QStringList::split(";", config_file.readEntry("Sounds", "SoundPaths")));
	selpaths->setPathList(pl);

	
};

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
	QComboBox* combobox=ConfigDialog::getComboBox("Sounds","Sound theme");
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
	soundmanager.playSound(item->text(1), e_soundprog->text());
};

void SoundSlots::chooseSoundTheme(const QString& string)
{
	kdebug("SoundSlots::chooseSoundTheme()\n");
	config_file.writeEntry("Sounds", "SoundTheme", string);
	soundmanager.setSoundTheme(config_file.readEntry("Sounds","SoundTheme"));

	QPushButton *choose = ConfigDialog::getPushButton("Sounds","Choose");
	QPushButton *clear = ConfigDialog::getPushButton("Sounds","Clear");

	QString chatfile;
	QString messagefile;
	QString notifyfile;

	QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
	lv_soundfiles->clear();
	    if (string == tr("Custom"))
		{
		 chatfile=config_file.readEntry("Sounds", "Chat_sound");
	         messagefile=config_file.readEntry("Sounds", "Message_sound");
		 notifyfile= config_file.readEntry("Notify", "NotifySound");
		 choose->setEnabled(true);
		 clear->setEnabled(true);
		}
	    else 
		{
		 chatfile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Chat");
	         messagefile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Message");
	         notifyfile= soundmanager.themePath(config_file.readEntry("Sounds", "SoundTheme"))+soundmanager.getThemeEntry("Notify");
		 choose->setEnabled(false);
		 clear->setEnabled(false);
		}

	    new QListViewItem(lv_soundfiles, tr("Chat sound"), chatfile);
	    new QListViewItem(lv_soundfiles, tr("Message sound"), messagefile);
	    new QListViewItem(lv_soundfiles, tr("Notify sound"), notifyfile);
};

void SoundSlots::selectedPaths(const QStringList& paths)
{
	soundmanager.setSoundPaths(paths);
	QComboBox* cb_soundtheme= ConfigDialog::getComboBox("Sounds","Sound theme");
	QString current= cb_soundtheme->currentText();
	cb_soundtheme->clear();
	cb_soundtheme->insertItem("Custom");// 0-wa pozycja
	cb_soundtheme->insertStringList(soundmanager.themes());
	cb_soundtheme->setCurrentText(current);
	cb_soundtheme->changeItem(tr("Custom"), 0);// dodanie translacji 
	config_file.writeEntry("Sounds", "SoundPaths", paths.join(";"));
};

void SoundSlots::onDestroyConfigDialog()
{
	kdebug("SoundSlots::onDestroyConfigDialog()\n");
	QComboBox *cb_soundtheme= ConfigDialog::getComboBox("Sounds", "Sound theme");
	QString theme;
	if (cb_soundtheme->currentText() == tr("Custom"))
	    {
		theme= "Custom";
		QListView* lv_soundfiles=ConfigDialog::getListView("Sounds", "sound_files");
		QListViewItem *item= lv_soundfiles->firstChild();
		config_file.writeEntry("Sounds", "Chat_sound", item->text(1));
		lv_soundfiles->takeItem(item);
		item= lv_soundfiles->firstChild();
		config_file.writeEntry("Sounds", "Message_sound", item->text(1));
		lv_soundfiles->takeItem(item);
		item= lv_soundfiles->firstChild();
		config_file.writeEntry("Notify", "NotifySound", item->text(1));
	    }
	else
	    theme= cb_soundtheme->currentText();
	    
	config_file.writeEntry("Sounds", "SoundTheme", theme);
};
