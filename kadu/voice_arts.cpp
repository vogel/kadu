/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmutex.h>

#include "debug.h"
#include "voice.h"
#include "voice_arts.h"

QMutex artsmutex;

VoiceArts::VoiceArts() {
	kdebug("VoiceArts::VoiceArts()\n");
	connect(voice_manager, SIGNAL(setupSoundDevice()), this, SLOT(setup()));
	connect(voice_manager, SIGNAL(freeSoundDevice()), this, SLOT(free()));
	connect(voice_manager, SIGNAL(playSample(char *, int)), this, SLOT(playSample(char *, int)));
	connect(voice_manager, SIGNAL(recordSample(char *, int)), this, SLOT(recordSample(char *, int)));
}

VoiceArts::~VoiceArts() {
}

void VoiceArts::setup() {
	artsmutex.lock();
	arts_init();
	playstream = arts_play_stream(8000, 16, 1, "kaduplayvoice");
	arts_stream_set(playstream, ARTS_P_BLOCKING, 0);
	kdebug("VoiceArts::setup(): playstream=%d\n", playstream);
	recstream = arts_record_stream(8000, 16, 1, "kadurecordvoice");
	arts_stream_set(recstream, ARTS_P_BLOCKING, 0);
	kdebug("VoiceArts::setup(): recstream=%d\n", recstream);
	artsmutex.unlock();
}

void VoiceArts::free() {
	artsmutex.lock();
	arts_close_stream(playstream);
	arts_close_stream(recstream);
	arts_free();
	kdebug("VoiceArts::free()\n");
	artsmutex.unlock();
}

void VoiceArts::playSample(char *data, int length) {
	int count = 0;
	kdebug("VoiceArts::playSample()\n");
	while (count < length) {
		artsmutex.lock();
		count += arts_write(playstream, data + count, length - count);
		artsmutex.unlock();
		}
}

void VoiceArts::recordSample(char *data, int length) {
	int count = 0;
	kdebug("VoiceArts::recordSample()\n");
	while (count < length) {
		artsmutex.lock();
		count += arts_read(recstream, data + count, length - count);
		artsmutex.unlock();
		}
}

VoiceArts *voice_arts;
