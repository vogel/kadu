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
	kdebugf();
	int value;
	
	kdebug("Opening /dev/dsp\n");
	fd = open("/dev/dsp", O_RDWR);
	if(fd<0)
	{
		kdebug("Error opening /dev/dsp\n");
		return;
	}

	kdebug("Setting speed for /dev/dsp\n");
	value = 8000;
	if(ioctl(fd, SNDCTL_DSP_SPEED, &value)<0)
	{
		kdebug("Error setting speed for /dev/dsp\n");
		return;
	}

	kdebug("Setting sample size for /dev/dsp\n");
	value = 16;
	if(ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &value)<0)
	{
		kdebug("Error setting sample size for /dev/dsp\n");
		return;
	}

	kdebug("Setting channels for /dev/dsp\n");
	value = 1;
	if(ioctl(fd, SNDCTL_DSP_CHANNELS, &value)<0)
	{
		kdebug("Error setting channels for /dev/dsp\n");
		return;
	}
	
	kdebug("Setting ftm for /dev/dsp\n");
	value = AFMT_S16_LE;
	if(ioctl(fd, SNDCTL_DSP_SETFMT, &value)<0)
	{
		kdebug("Error setting ftm for /dev/dsp\n");
		return;
	}
	kdebug("Setup successful, fd=%d\n", fd);
}

void VoiceDsp::free() {
	kdebugf();
	if(fd>=0)
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

VoiceDsp* voice_dsp = NULL;
