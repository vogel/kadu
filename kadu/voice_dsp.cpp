/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "debug.h"
#include "voice.h"
#include "voice_dsp.h"

SoundDsp::SoundDsp() {
	kdebug("SoundDsp::SoundDsp()\n");
	connect(voice_manager, SIGNAL(setupSoundDevice()), this, SLOT(setup()));
	connect(voice_manager, SIGNAL(freeSoundDevice()), this, SLOT(free()));
	connect(voice_manager, SIGNAL(playSample(char *, int)), this, SLOT(playSample(char *, int)));
	connect(voice_manager, SIGNAL(recordSample(char *, int)), this, SLOT(recordSample(char *, int)));
}

SoundDsp::~SoundDsp() {
}

void SoundDsp::setup() {
	int value;	
	fd = open("/dev/dsp", O_RDWR);
	value = 8000;
	ioctl(fd, SNDCTL_DSP_SPEED, &value);
	value = 16;
	ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &value);
	value = 1;
	ioctl(fd, SNDCTL_DSP_CHANNELS, &value);
	value = AFMT_S16_LE;
	ioctl(fd, SNDCTL_DSP_SETFMT, &value);
	kdebug("SoundDsp::setup(): fd=%d\n", fd);
}

void SoundDsp::free() {
	kdebug("SoundDsp::free()\n");
	close(fd);
}

void SoundDsp::playSample(char *data, int length) {
	kdebug("SoundDsp::playSample()\n");
	write(fd, data, length);
}

void SoundDsp::recordSample(char *data, int length) {
	kdebug("SoundDsp::recordSample()\n");
	read(fd, data, length);
}

SoundDsp *sound_dsp;

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
