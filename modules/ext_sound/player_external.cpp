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
#include <stdlib.h>

extern "C" int ext_sound_init()
{
	kdebugf();

	externalPlayerObj=new ExternalPlayerSlots();

	ConfigDialog::addHGroupBox("Sounds", "Sounds", 	QT_TRANSLATE_NOOP("@default","Sound player"));	
	ConfigDialog::addLineEdit("Sounds", "Sound player",
			QT_TRANSLATE_NOOP("@default","Path:"), "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "OpenFile","","soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), externalPlayerObj, SLOT(choosePlayerFile()), "soundplayer_fileopen");

	kdebugf2();
	return 0;
}
extern "C" void ext_sound_close()
{
	kdebugf();

	ConfigDialog::disconnectSlot("Sounds", "", SIGNAL(clicked()), externalPlayerObj, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	ConfigDialog::removeControl("Sounds", "", "soundplayer_fileopen");
	ConfigDialog::removeControl("Sounds", "Path:", "soundplayer_path");
	ConfigDialog::removeControl("Sounds", "Sound player");
	delete externalPlayerObj;
	externalPlayerObj=NULL;
	kdebugf2();
}

ExternalPlayerSlots::ExternalPlayerSlots()
{
	kdebugf();

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNewMessage(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewMessage(UinsList, const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnNewChat(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewChat(UinsList, const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnConnectionError(const QString &, bool, double, const QString &)),
			this, SLOT(playConnectionError(const QString &, bool, double, const QString &)));
	connect(sound_manager, SIGNAL(playOnNotifyAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNotifyBusy(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnNotifyNotAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	connect(sound_manager, SIGNAL(playOnMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)),
			this, SLOT(playMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)));

	kdebugf2();
}

ExternalPlayerSlots::~ExternalPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNewMessage(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewMessage(UinsList, const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnNewChat(UinsList, const QString &, bool, double, const QString &)),
			this, SLOT(playNewChat(UinsList, const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnConnectionError(const QString &, bool, double, const QString &)),
			this, SLOT(playConnectionError(const QString &, bool, double, const QString &)));
	disconnect(sound_manager, SIGNAL(playOnNotifyAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNotifyBusy(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnNotifyNotAvail(const UinType, const QString &, bool, double)),
			this, SLOT(playNotify(const UinType, const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(playOnMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)),
			this, SLOT(playMessage(const QString &, bool, double, const QString &, const QString &, const QString &, const UserListElement *)));

	kdebugf2();
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
	kdebugm(KDEBUG_INFO, "%s\n", t.ascii());
	system(t.ascii());
	kdebugf2();
}

void ExternalPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString player=QString::null;
	if (ConfigDialog::dialogOpened())
	{
		QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");
		player=e_soundprog->text();
	}
	play(s, volCntrl, vol, player);
}

void ExternalPlayerSlots::playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void ExternalPlayerSlots::choosePlayerFile()
{
	kdebugf();
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");

	QString s(QFileDialog::getOpenFileName( e_soundprog->text(), "All Files (*)"));
	if (s.length())
		e_soundprog->setText(s);
	kdebugf2();
}

ExternalPlayerSlots *externalPlayerObj;

