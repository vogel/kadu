/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include "player_external.h"
#include "debug.h"
#include "config_dialog.h"

SoundManager* sound_manager;

extern "C" void ext_sound_info(ModuleInfo* i)
{
    i->description="External application sound module";
    i->author="Joi";
    i->depends+="sound";
}

extern "C" int ext_sound_init()
{
	kdebugf();
	QT_TRANSLATE_NOOP("@default","Sound player");
	QT_TRANSLATE_NOOP("@default","Path:");
	sound_manager=soundManager();
	if(sound_manager==NULL)
		return 1;
	slotsObj=new ExternalPlayerSlots();

	QObject::connect(sound_manager, SIGNAL(playOnTestSound(const QString &, bool, double)),
					 slotsObj, SLOT(playTestSound(const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
					 slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
					 slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));

	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Sound player");
	ConfigDialog::addLineEdit("Sounds", "Sound player", "Path:", "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "OpenFile","","soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), slotsObj, SLOT(choosePlayerFile()), "soundplayer_fileopen");

	return 0;
}
extern "C" void ext_sound_close()
{
	kdebugf();

	ConfigDialog::disconnectSlot("Sounds", "", SIGNAL(clicked()), slotsObj, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	QObject::disconnect(sound_manager, SIGNAL(playOnTestSound(const QString &, bool, double)),
						slotsObj, SLOT(playTestSound(const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
						slotsObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
						slotsObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));
	ConfigDialog::removeControl("Sounds", "", "soundplayer_fileopen");
	ConfigDialog::removeControl("Sounds", "Path:", "soundplayer_path");
	ConfigDialog::removeControl("Sounds", "Sound player");
	delete slotsObj;
	slotsObj=NULL;
}

ExternalPlayerSlots::ExternalPlayerSlots()
{
	kdebugf();
}

void ExternalPlayerSlots::play(const QString &s, bool volCntrl, double vol, QString player)
{
	kdebugf();
	QString t;

	if (player==QString::null)
		t=config_file.readEntry("Sounds","SoundPlayer", "/usr/bin/play");
	else
		t=player;

	if (volCntrl)
		t.append(" -v "+QString::number(vol, 'g', 2));
	t.append(" "+s+" >/dev/null &");
	kdebug("%s\n", t.ascii());
	system(t.ascii());
}

void ExternalPlayerSlots::playTestSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");
	play(s, volCntrl, vol, e_soundprog->text());
}

void ExternalPlayerSlots::playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::choosePlayerFile()
{
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");

	QString s(QFileDialog::getOpenFileName( e_soundprog->text(), "All Files (*)"));
	if (s.length())
		e_soundprog->setText(s);
}

ExternalPlayerSlots *slotsObj;

