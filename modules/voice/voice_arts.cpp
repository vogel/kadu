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
	kdebugf();
	connect(voice_manager, SIGNAL(setupSoundDevice()), this, SLOT(setup()));
	connect(voice_manager, SIGNAL(freeSoundDevice()), this, SLOT(free()));
	connect(voice_manager, SIGNAL(playSample(char *, int)), this, SLOT(playSample(char *, int)));
	connect(voice_manager, SIGNAL(recordSample(char *, int)), this, SLOT(recordSample(char *, int)));
	kdebugf2();
}

VoiceArts::~VoiceArts() {
}

void VoiceArts::setup() {
	kdebugf();
	artsmutex.lock();
	arts_init();
	playstream = arts_play_stream(8000, 16, 1, "kaduplayvoice");
	arts_stream_set(playstream, ARTS_P_BLOCKING, 0);
	arts_stream_set(playstream, ARTS_P_BUFFER_SIZE, 1600);
	kdebugmf(KDEBUG_INFO, "playstream=%d\n", playstream);
	recstream = arts_record_stream(8000, 16, 1, "kadurecordvoice");
	arts_stream_set(recstream, ARTS_P_BLOCKING, 0);
	kdebugmf(KDEBUG_INFO, "recstream=%d\n", recstream);
	artsmutex.unlock();
	kdebugf2();
}

void VoiceArts::free() {
	kdebugf();
	artsmutex.lock();
	arts_close_stream(playstream);
	arts_close_stream(recstream);
	arts_free();
	artsmutex.unlock();
	kdebugf2();
}

void VoiceArts::playSample(char *data, int length) {
	kdebugf();
	int count = 0;
	while (count < length)
	{
		artsmutex.lock();
		count += arts_write(playstream, data + count, length - count);
		artsmutex.unlock();
	}
	kdebugf2();
}

void VoiceArts::recordSample(char *data, int length) {
	kdebugf();
	int count = 0;
	while (count < length)
	{
		artsmutex.lock();
		count += arts_read(recstream, data + count, length - count);
		artsmutex.unlock();
	}
	kdebugf2();
}

VoiceArts *voice_arts;
