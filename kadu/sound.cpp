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

#include "sound.h"
#include "debug.h"
#include "config_file.h"
#include "config_dialog.h"

bool mute = false;

void playSound(const QString &sound, const QString player) {
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
}

PlayThread::PlayThread(SoundDevice *snddev) : QThread(), snddev(snddev) {
	semwait = new QSemaphore(1);
	semplay = new QSemaphore(1);
}

PlayThread::~PlayThread() {
	delete semwait;
	delete semplay;
}

void PlayThread::run() {
	kdebug("PlayThread::run()\n");
	while (true) {
		(*semwait)++;
		kdebug("PlayThread::run(): wokenUp\n");
		(*semplay)++;
		snddev->doPlaying();
		snddev->playfinished = true;
		(*semplay)--;
		}
}

RecordThread::RecordThread(SoundDevice *snddev) : QThread(), snddev(snddev) {
	semwait = new QSemaphore(1);
	semrec = new QSemaphore(1);
}

RecordThread::~RecordThread() {
	delete semwait;
	delete semrec;
}

void RecordThread::run() {
	kdebug("RecordThread::run()\n");
	while (true) {
		(*semwait)++;		
		(*semrec)++;
		snddev->doRecording();
		snddev->recfinished = true;
		(*semrec)--;
		}
}

SoundDevice::SoundDevice(const int freq, const int bits, const int chans, QObject *parent, const char *name)
	: QObject(parent, name), freq(freq), bits(bits), chans(chans) {
	pt = new PlayThread(this);
	(*pt->semwait)++;
	pt->start();
	rt = new RecordThread(this);
	(*rt->semwait)++;
	rt->start();
}

SoundDevice::~SoundDevice() {
	delete pt;
	delete rt;
}

void SoundDevice::play(char *buf, int size) {
	playbuf = buf;
	playbufsize = size;
	(*pt->semplay)++;
	playfinished = false;
	(*pt->semplay)--;
	(*pt->semwait)--;
}

bool SoundDevice::playFinished() {
	bool ret;
	(*pt->semplay)++;
	ret = playfinished;
	(*pt->semplay)--;
	return ret;
}

void SoundDevice::record(char *buf, int size) {
	recbuf = buf;
	recbufsize = size;
	(*rt->semrec)++;
	recfinished = false;
	(*rt->semrec)--;
	(*rt->semwait)--;
}

bool SoundDevice::recordFinished() {
	bool ret;
	(*rt->semrec)++;	
	ret = recfinished;
	(*rt->semrec)--;
	return ret;
}

void SoundSlots::initModule()
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


	ConfigDialog::registerTab("Sounds");
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds", "PlaySound", false);
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds using aRts! server", "PlaySoundArtsDsp", false);
	
	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Sound player");
	ConfigDialog::addLineEdit("Sounds", "Sound player", "Path:", "SoundPlayer","","","soundplayer_path");
	ConfigDialog::addPushButton("Sounds", "Sound player", "", "fileopen.png","","soundplayer_fileopen");
	
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Enable volume control (player must support it)", "VolumeControl", false);
	ConfigDialog::addGrid("Sounds", "Sounds", "volume", 2);
	ConfigDialog::addLabel("Sounds", "volume", "Volume");
	ConfigDialog::addSlider("Sounds", "volume", "slider", "SoundVolume", 0, 400, 50, 200);

	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Message sound");
	ConfigDialog::addLineEdit("Sounds", "Message sound", "Path:", "Message_sound","","","messagesound_path");
	ConfigDialog::addPushButton("Sounds", "Message sound", "", "fileopen.png","","messagesound_fileopen");
	ConfigDialog::addPushButton("Sounds", "Message sound", "Test","","","messagesound_test");

	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play sounds from a person whilst chatting", "PlaySoundChat", true);
	ConfigDialog::addCheckBox("Sounds", "Sounds", "Play chat sounds only when window is invisible", "PlaySoundChatInvisible", true);
	
	ConfigDialog::addHGroupBox("Sounds", "Sounds", "Chat sound");
	ConfigDialog::addLineEdit("Sounds", "Chat sound", "Path:", "Chat_sound","","","chatsound_path");
	ConfigDialog::addPushButton("Sounds", "Chat sound", "", "fileopen.png","","chatsound_fileopen");
	ConfigDialog::addPushButton("Sounds", "Chat sound", "Test","","","chatsound_test");
	
	SoundSlots *soundslots= new SoundSlots();
	ConfigDialog::registerSlotOnCreate(soundslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::connectSlot("Sounds", "Play sounds", SIGNAL(toggled(bool)), soundslots, SLOT(soundPlayer(bool)));
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), soundslots, SLOT(choosePlayerFile()), "soundplayer_fileopen");
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), soundslots, SLOT(chooseMsgFile()), "messagesound_fileopen");
	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(clicked()), soundslots, SLOT(chooseMsgTest()), "messagesound_test");
	ConfigDialog::connectSlot("Sounds", "", SIGNAL(clicked()), soundslots, SLOT(chooseChatFile()), "chatsound_fileopen");
	ConfigDialog::connectSlot("Sounds", "Test", SIGNAL(clicked()), soundslots, SLOT(chooseChatTest()), "chatsound_test");

};

void SoundSlots::onCreateConfigDialog()
{
	kdebug("SoundSlots::onCreateConfigDialog()\n");
	QCheckBox *b_playsound= ConfigDialog::getCheckBox("Sounds", "Play sounds");
	QCheckBox *b_playarts= ConfigDialog::getCheckBox("Sounds", "Play sounds using aRts! server");
	QHGroupBox *g_soundplayer= ConfigDialog::getHGroupBox("Sounds", "Sound player");
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QGrid *g_volume= ConfigDialog::getGrid("Sounds","volume");
	QCheckBox *b_playchatting= ConfigDialog::getCheckBox("Sounds", "Play sounds from a person whilst chatting");
	QCheckBox *b_playinvisible= ConfigDialog::getCheckBox("Sounds", "Play chat sounds only when window is invisible");
	QHGroupBox *g_messagesnd= ConfigDialog::getHGroupBox("Sounds", "Message sound");
	QHGroupBox *g_chatsnd= ConfigDialog::getHGroupBox("Sounds", "Chat sound");
	

	soundPlayer(b_playsound->isChecked());

	g_volume->setEnabled(b_playsound->isChecked() && b_volumectrl->isChecked());
	b_playinvisible->setEnabled(b_playsound->isChecked()&& b_playchatting->isChecked());
	g_chatsnd->setEnabled(b_playsound->isChecked()&& b_playchatting->isChecked());
	g_chatsnd->setEnabled(b_playsound->isChecked()&& b_playinvisible->isChecked());
	
	connect(b_volumectrl,SIGNAL(toggled(bool)), g_volume, SLOT(setEnabled(bool)));
	connect(b_playchatting,SIGNAL(toggled(bool)), b_playinvisible, SLOT(setEnabled(bool)));
	connect(b_playchatting,SIGNAL(toggled(bool)), g_chatsnd, SLOT(setEnabled(bool)));	
	connect(b_playinvisible,SIGNAL(toggled(bool)), g_chatsnd, SLOT(setEnabled(bool)));
	
};

void SoundSlots::soundPlayer(bool value)
{

	QCheckBox *b_playsound= ConfigDialog::getCheckBox("Sounds", "Play sounds");
	QCheckBox *b_playarts= ConfigDialog::getCheckBox("Sounds", "Play sounds using aRts! server");
	QHGroupBox *g_soundplayer= ConfigDialog::getHGroupBox("Sounds", "Sound player");
	QCheckBox *b_volumectrl= ConfigDialog::getCheckBox("Sounds", "Enable volume control (player must support it)");
	QGrid *g_volume= ConfigDialog::getGrid("Sounds","volume");
	QCheckBox *b_playchatting= ConfigDialog::getCheckBox("Sounds", "Play sounds from a person whilst chatting");
	QCheckBox *b_playinvisible= ConfigDialog::getCheckBox("Sounds", "Play chat sounds only when window is invisible");
	QHGroupBox *g_messagesnd= ConfigDialog::getHGroupBox("Sounds", "Message sound");
	QHGroupBox *g_chatsnd= ConfigDialog::getHGroupBox("Sounds", "Chat sound");

	    b_playarts->setEnabled(value);
	    g_soundplayer->setEnabled(value);
	    b_volumectrl->setEnabled(value);
	    g_volume->setEnabled(value);
	    g_messagesnd->setEnabled(value);
	    g_chatsnd->setEnabled(value);
	    b_playchatting->setEnabled(value);
	    b_playinvisible->setEnabled(value);

}

void SoundSlots::choosePlayerFile()
{
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");

	QString s(QFileDialog::getOpenFileName( QString::null, "All Files (*)"));
	    if (s.length())
		    e_soundprog->setText(s);
		
};

void SoundSlots::chooseMsgFile()
{
	QLineEdit *e_msgfile= ConfigDialog::getLineEdit("Sounds", "Path:", "messagesound_path");

	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)"));
	    if (s.length())
		    e_msgfile->setText(s);
		
};

void SoundSlots::chooseMsgTest()
{
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");
	QLineEdit *e_msgfile= ConfigDialog::getLineEdit("Sounds", "Path:", "messagesound_path");
	
	playSound(e_msgfile->text(), e_soundprog->text());
};

void SoundSlots::chooseChatFile()
{
	QLineEdit *e_chatfile=  ConfigDialog::getLineEdit("Sounds", "Path:", "chatsound_path");

	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)"));
	    if (s.length())
		    e_chatfile->setText(s);
		
};


void SoundSlots::chooseChatTest()
{
	QLineEdit *e_soundprog= ConfigDialog::getLineEdit("Sounds", "Path:", "soundplayer_path");
	QLineEdit *e_chatfile=  ConfigDialog::getLineEdit("Sounds", "Path:", "chatsound_path");

	playSound(e_chatfile->text(), e_soundprog->text());
};


/*ArtsSoundDevice::ArtsSoundDevice(const int freq, const int bits, const int chans, QObject *parent, const char *name)
	: SoundDevice(freq, bits, chans, parent, name) {
	arts_init();
	playstream = arts_play_stream(freq, bits, chans, "kaduplayvoice");
	kdebug("ArtsSoundDevice::ArtsSoundDevice(): playstream=%d\n", playstream);
	arts_stream_set(playstream, ARTS_P_BUFFER_SIZE, 1024);
	recstream = arts_record_stream(freq, bits, chans, "kadurecordvoice");
	kdebug("ArtsSoundDevice::ArtsSoundDevice(): recstream=%d\n", recstream);
}

ArtsSoundDevice::~ArtsSoundDevice() {
	arts_close_stream(playstream);
	arts_close_stream(recstream);
	arts_free();
}

void ArtsSoundDevice::doPlaying() {
	kdebug("ArtsSoundDevice::doPlaying(): playstream=%d\n", playstream);
	arts_write(playstream, playbuf, playbufsize);
}

void ArtsSoundDevice::doRecording() {
	kdebug("ArtsSoundDevice::doRecording(): recstream=%d\n", recstream);
	arts_read(recstream, recbuf, recbufsize);
}*/

/*DspSoundDevice::DspSoundDevice(const int freq, const int bits, const int chans, QObject *parent, const char *name)
	: SoundDevice(freq, bits, chans, parent, name) {
	int value;	
	fd = open("/dev/dsp", O_RDWR);
	value = freq;
	ioctl(fd, SNDCTL_DSP_SPEED, &value);
//	value = bits;
//	ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &value);
	value = chans;
	ioctl(fd, SNDCTL_DSP_CHANNELS, &value);
//	value = 16000;
//	ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &value);
//	value = AFMT_S16_LE;
	value = AFMT_U8;
	ioctl(fd, SNDCTL_DSP_SETFMT, &value);
	kdebug("DspSoundDevice::DspSoundDevice(): fd=%d\n", fd);
}

DspSoundDevice::~DspSoundDevice() {
	close(fd);
}

void DspSoundDevice::doPlaying() {
	kdebug("DspSoundDevice::doPlaying(): fd=%d\n", fd);
	write(fd, playbuf, playbufsize);
}

void DspSoundDevice::doRecording() {
	kdebug("DspSoundDevice::doRecording(): fd=%d\n", fd);
	read(fd, recbuf, recbufsize);
}*/
