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
	while (true) {
		(*semwait)++;
		(*semplay)++;
		snddev->doPlaying();
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
	while (true) {
		(*semwait)++;		
		(*semrec)++;
		snddev->doRecording();
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
