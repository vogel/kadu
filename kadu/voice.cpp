/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "libgadu.h"
#include "voice.h"

PlayThread::PlayThread(): wsem(32) {
	wsem += 32;
}

void PlayThread::run() {
	kdebug("PlayThread::run()\n");
	struct gsm_sample gsmsample;
	while (true) {
		wsem++;
		kdebug("PlayThread::run(): wokenUp\n");
		mutex.lock();
		gsmsample = queue.front();
		queue.pop_front();
		mutex.unlock();
		emit playGsmSample(gsmsample.data, gsmsample.length);
		}
}

RecordThread::RecordThread(): wsem(32) {
	wsem += 32;
}

void RecordThread::run() {
	kdebug("RecordThread::run()\n");
	char data[GG_DCC_VOICE_FRAME_LENGTH_505];
	int length = GG_DCC_VOICE_FRAME_LENGTH_505;
	while (true) {
		emit recordSample(data, length);
		}
}

VoiceManager::VoiceManager() {
	kdebug("VoiceManager::VoiceManager()\n");
	gsm_enc = gsm_dec = NULL;
	connect(&pt, SIGNAL(playGsmSample(char *, int)), this, SLOT(playGsmSampleReceived(char *, int)));
	connect(&rt, SIGNAL(recordSample(char *, int)), this, SLOT(recordSampleReceived(char *, int)));
}

void VoiceManager::setup() {
	emit setupSoundDevice();
	pt.start();
//	rt.start();
}

void VoiceManager::free() {
	pt.exit();
//	rt.exit();
	emit freeSoundDevice();
}

void VoiceManager::resetCodec() {
	kdebug("VoiceManager::resetCodec()\n");
	int value = 1;
	if (gsm_enc)
		gsm_destroy(voice_enc);
	if (gsm_dec)
		gsm_destroy(voice_dec);
	voice_enc = gsm_create();
	gsm_option(voice_enc, GSM_OPT_FAST, &value);
	gsm_option(voice_enc, GSM_OPT_WAV49, &value);
	voice_dec = gsm_create();
	gsm_option(voice_dec, GSM_OPT_FAST, &value);
	gsm_option(voice_dec, GSM_OPT_WAV49, &value);
	gsm_option(voice_dec, GSM_OPT_VERBOSE, &value);
	gsm_option(voice_dec, GSM_OPT_LTP_CUT, &value);
}

void VoiceManager::playGsmSampleReceived(char *data, int length) {
	kdebug("VoiceManager::playGsmSampleReceived()\n");
	const char *pos = data;
	int outlen = 320;
	gsm_signal output[160];
	resetCodec();
	while (pos <= (data + length - 65)) {
		gsm_decode(voice_dec, (char *) pos, output);
		pos += 33;
		emit playSample(output, outlen);
		gsm_decode(voice_dec, (char *) pos, output);
		pos += 32;
		emit playSample(output, outlen);
		}
}

void VoiceManager::recordSampleReceived(char *data, int length) {
	kdebug("VoiceManager::recordSampleReceived()\n");
	const char *pos = data;
	int inlen = 320;
	gsm_signal input[160];
	resetCodec();
	*data = 0;
	data++;
	pos++;
	length--;
	while (pos <= (data + length - 65)) {
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (char *) pos);
		pos += 32;
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (char *) pos);
		pos += 33;
		}
	emit gsmSampleRecorded(data, length);
}

void VoiceManager::addGsmSample(char *data, int length) {
	kdebug("VoiceManager::addGsmSample()\n");
	struct gsm_sample gsmsample;
	gsmsample.data = data;
	gsmsample.length = length;
	mutex.lock();
	queue << gsmsample;
	mutex.unlock();
	pt.wsem--;
}
