/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qobject.h>
#include <qthread.h>
#include <qsemaphore.h>
//#include <kde/artsc/artsc.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <linux/soundcard.h>
//#include <unistd.h>

#include "sound.h"

PlayThread::PlayThread(SoundDevice *snddev) : QThread(), snddev(snddev) {
	semwait = new QSemaphore(1);
	semplay = new QSemaphore(1);
}

PlayThread::~PlayThread() {
	delete semwait;
	delete semplay;
}

void PlayThread::run() {
	fprintf(stderr, "KK PlayThread::run()\n");
	while (true) {
		(*semwait)++;
		fprintf(stderr, "KK PlayThread::run(): wokenUp\n");
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
	fprintf(stderr, "KK RecordThread::run()\n");
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

/*ArtsSoundDevice::ArtsSoundDevice(const int freq, const int bits, const int chans, QObject *parent, const char *name)
	: SoundDevice(freq, bits, chans, parent, name) {
	arts_init();
	playstream = arts_play_stream(freq, bits, chans, "kaduplayvoice");
	fprintf(stderr, "KK ArtsSoundDevice::ArtsSoundDevice(): playstream=%d\n", playstream);
	arts_stream_set(playstream, ARTS_P_BUFFER_SIZE, 1024);
	recstream = arts_record_stream(freq, bits, chans, "kadurecordvoice");
	fprintf(stderr, "KK ArtsSoundDevice::ArtsSoundDevice(): recstream=%d\n", recstream);
}

ArtsSoundDevice::~ArtsSoundDevice() {
	arts_close_stream(playstream);
	arts_close_stream(recstream);
	arts_free();
}

void ArtsSoundDevice::doPlaying() {
	fprintf(stderr, "KK ArtsSoundDevice::doPlaying(): playstream=%d\n", playstream);
	arts_write(playstream, playbuf, playbufsize);
}

void ArtsSoundDevice::doRecording() {
	fprintf(stderr, "KK ArtsSoundDevice::doRecording(): recstream=%d\n", recstream);
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
	fprintf(stderr, "KK DspSoundDevice::DspSoundDevice(): fd=%d\n", fd);
}

DspSoundDevice::~DspSoundDevice() {
	close(fd);
}

void DspSoundDevice::doPlaying() {
	fprintf(stderr, "KK DspSoundDevice::doPlaying(): fd=%d\n", fd);
	write(fd, playbuf, playbufsize);
}

void DspSoundDevice::doRecording() {
	fprintf(stderr, "KK DspSoundDevice::doRecording(): fd=%d\n", fd);
	read(fd, recbuf, recbufsize);
}*/
