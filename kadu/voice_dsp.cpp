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

VoiceDsp::VoiceDsp() {
	kdebug("VoiceDsp::VoiceDsp()\n");
	connect(voice_manager, SIGNAL(setupSoundDevice()), this, SLOT(setup()));
	connect(voice_manager, SIGNAL(freeSoundDevice()), this, SLOT(free()));
	connect(voice_manager, SIGNAL(playSample(char *, int)), this, SLOT(playSample(char *, int)));
	connect(voice_manager, SIGNAL(recordSample(char *, int)), this, SLOT(recordSample(char *, int)));
}

VoiceDsp::~VoiceDsp() {
}

void VoiceDsp::setup() {
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
	kdebug("VoiceDsp::setup(): fd=%d\n", fd);
}

void VoiceDsp::free() {
	kdebug("VoiceDsp::free()\n");
	close(fd);
}

void VoiceDsp::playSample(char *data, int length) {
	kdebug("VoiceDsp::playSample()\n");
	write(fd, data, length);
}

void VoiceDsp::recordSample(char *data, int length) {
	kdebug("VoiceDsp::recordSample()\n");
	read(fd, data, length);
}

VoiceDsp *voice_dsp;
