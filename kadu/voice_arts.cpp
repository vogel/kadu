/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "voice.h"
#include "voice_arts.h"

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
	arts_init();
	playstream = arts_play_stream(8000, 16, 1, "kaduplayvoice");
	kdebug("VoiceArts::setup(): playstream=%d\n", playstream);
//	arts_stream_set(playstream, ARTS_P_BUFFER_SIZE, 1024);
	recstream = arts_record_stream(8000, 16, 1, "kadurecordvoice");
	kdebug("VoiceArts::setup(): recstream=%d\n", recstream);
}

void VoiceArts::free() {
	kdebug("VoiceArts::free()\n");
	arts_close_stream(playstream);
	arts_close_stream(recstream);
	arts_free();
}

void VoiceArts::playSample(char *data, int length) {
	kdebug("VoiceArts::playSample()\n");
	arts_write(playstream, data, length);
}

void VoiceArts::recordSample(char *data, int length) {
	kdebug("VoiceArts::recordSample(): read = %d\n",
		arts_read(recstream, data, length));
}

VoiceArts *voice_arts;
