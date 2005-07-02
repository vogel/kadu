/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include "../sound/sound.h"
#include "player_external.h"
#include "debug.h"
#include "config_dialog.h"
#include <stdlib.h>

extern "C" int ext_sound_init()
{
	kdebugf();
	external_player_slots=new ExternalPlayerSlots(NULL, "external_player_slots");
	kdebugf2();
	return 0;
}
extern "C" void ext_sound_close()
{
	kdebugf();
	delete external_player_slots;
	external_player_slots=NULL;
	kdebugf2();
}

ExternalPlayerSlots::ExternalPlayerSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

#ifdef Q_OS_MACX
	config_file.addVariable("Sounds", "SoundPlayer", "/Applications/kadu.app/Contents/MacOS/playsound");
#else
	config_file.addVariable("Sounds", "SoundPlayer", "/usr/bin/play");
#endif
	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	ConfigDialog::addHGroupBox("Sounds", "Sounds", QT_TRANSLATE_NOOP("@default","Sound player"), "", Advanced);
	ConfigDialog::addLineEdit("Sounds", "Sound player",
			QT_TRANSLATE_NOOP("@default","Path:"), "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "OpenFile","","soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), this, SLOT(choosePlayerFile()), "soundplayer_fileopen");

	kdebugf2();
}

ExternalPlayerSlots::~ExternalPlayerSlots()
{
	kdebugf();

	ConfigDialog::disconnectSlot("Sounds", "", SIGNAL(clicked()), this, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	ConfigDialog::removeControl("Sounds", "", "soundplayer_fileopen");
	ConfigDialog::removeControl("Sounds", "Path:", "soundplayer_path");
	ConfigDialog::removeControl("Sounds", "Sound player");

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

void ExternalPlayerSlots::play(const QString &s, bool volCntrl, double vol, QString player)
{
	kdebugf();
	QString t;

	if (player==QString::null)
		t=config_file.readEntry("Sounds", "SoundPlayer");
	else
		t=player;
	
	if (t.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: player path is empty\n");
		return;
	}
	
	if (volCntrl)
		t.append(" -v "+QString::number(vol, 'g', 2));
	t.append(" "+s+" >/dev/null &");
	kdebugm(KDEBUG_INFO, "%s\n", t.ascii());
	system(t.ascii());
	kdebugf2();
}

void ExternalPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString player=QString::null;
	if (ConfigDialog::dialogOpened())
		player=ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path")->text();
	play(s, volCntrl, vol, player);
}

void ExternalPlayerSlots::choosePlayerFile()
{
	kdebugf();
	QLineEdit *playerPath= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");

	QString s(QFileDialog::getOpenFileName( playerPath->text(), "All Files (*)", ConfigDialog::configdialog));
	if (!s.isEmpty())
		playerPath->setText(s);
	kdebugf2();
}

ExternalPlayerSlots *external_player_slots;

