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

#include "sound.h"

PlayThread::PlayThread(SoundDevice *snddev) : QThread(), snddev(snddev) {
}

void PlayThread::run() {
}

RecordThread::RecordThread(SoundDevice *snddev) : QThread(), snddev(snddev) {
}

void RecordThread::run() {
}

SoundDevice::SoundDevice(int freq, int bits, int chans, QObject *parent, const char *name)
	: QObject(parent, name), freq(freq), bits(bits), chans(chans) {
	pt = new PlayThread(this);
	rt = new RecordThread(this);
}
