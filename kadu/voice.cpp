/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "voice.h"

#include "voice_dsp.h"
#include "debug.h"
#include "libgadu.h"

PlayThread::PlayThread(): wsem(32), rsem(1) {
	wsem += 32;
}

void PlayThread::run() {
	struct gsm_sample gsmsample;
	kdebug("PlayThread::run()\n");
	while (true) {
		kdebug("PlayThread(): rsem = %d\n", rsem.available());
		if (!rsem.available())
			break;
		wsem++;
		kdebug("PlayThread::run(): wokenUp\n");
		mutex.lock();
		if (queue.empty()) {
			mutex.unlock();
			continue;
			}
		gsmsample = queue.front();
		queue.pop_front();
		mutex.unlock();
		emit playGsmSample(gsmsample.data, gsmsample.length);
		delete gsmsample.data;
		}
	kdebug("PlayThread::run(): exiting ...\n");
}

RecordThread::RecordThread(): rsem(1) {
}

void RecordThread::run() {
	kdebug("RecordThread::run()\n");
	char data[GG_DCC_VOICE_FRAME_LENGTH_505];
	int length = GG_DCC_VOICE_FRAME_LENGTH_505;
	while (true) {
		if (!rsem.available())
			break;
		emit recordSample(data, length);
		}
	kdebug("RecordThread::run(): exiting ...\n");
}

VoiceManager::VoiceManager() {
	kdebug("VoiceManager::VoiceManager()\n");
	voice_enc = voice_dec = NULL;
	pt = new PlayThread();
	rt = new RecordThread();
	connect(pt, SIGNAL(playGsmSample(char *, int)), this, SLOT(playGsmSampleReceived(char *, int)));
	connect(rt, SIGNAL(recordSample(char *, int)), this, SLOT(recordSampleReceived(char *, int)));
}

void VoiceManager::setup() {
	kdebug("VoiceManager::setup()\n");
	if (!pt->running()) {
		emit setupSoundDevice();
		pt->rsem--;
		pt->start();
		}
	if (!rt->running()) {
		rt->rsem--;
		rt->start();
		}
}

void VoiceManager::free() {
	struct gsm_sample gsmsample;
	kdebug("VoiceManager::free()\n");
	if (rt->running())
		rt->rsem++;
	if (pt->running()) {
		pt->wsem--;
		pt->rsem++;
		pt->mutex.lock();
		while (!pt->queue.empty()) {
			gsmsample = pt->queue.front();
			pt->queue.pop_front();
			delete gsmsample.data;
			}
		pt->mutex.unlock();
		emit freeSoundDevice();
		}
}

void VoiceManager::resetCodec() {
	kdebug("VoiceManager::resetCodec()\n");
	resetCoder();
	resetDecoder();
}

void VoiceManager::resetCoder() {
	kdebug("VoiceManager::resetCoder()\n");
	int value = 1;
	if (voice_enc)
		gsm_destroy(voice_enc);
	voice_enc = gsm_create();
	gsm_option(voice_enc, GSM_OPT_FAST, &value);
	gsm_option(voice_enc, GSM_OPT_WAV49, &value);
}

void VoiceManager::resetDecoder() {
	kdebug("VoiceManager::resetDecoder()\n");
	int value = 1;
	if (voice_dec)
		gsm_destroy(voice_dec);
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
	resetDecoder();
	data++;
	pos++;
	length--;
	while (pos <= (data + length - 65)) {
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output)) {
			kdebug("VoiceManager::playGsmSampleReceived(): gsm_decode() error\n");
			return;
			}
		pos += 33;
		emit playSample((char *) output, outlen);
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output)) {
			kdebug("VoiceManager::playGsmSampleReceived(): gsm_decode() error\n");
			return;
			}
		pos += 32;
		emit playSample((char *) output, outlen);
		}
}

void VoiceManager::recordSampleReceived(char *data, int length) {
	kdebug("VoiceManager::recordSampleReceived()\n");
	const char *pos = data;
	int inlen = 320;
	gsm_signal input[160];
	resetCoder();
	*data = 0;
	data++;
	pos++;
	length--;
	while (pos <= (data + length - 65)) {
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (gsm_byte *) pos);
		pos += 32;
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (gsm_byte *) pos);
		pos += 33;
		}
	emit gsmSampleRecorded(data - 1, length + 1);
}

void VoiceManager::addGsmSample(char *data, int length) {
	kdebug("VoiceManager::addGsmSample()\n");
	struct gsm_sample gsmsample;
	gsmsample.data = data;
	gsmsample.length = length;
	pt->mutex.lock();
	pt->queue << gsmsample;
	pt->mutex.unlock();
	pt->wsem--;
}

void VoiceManager::initModule()
{
	voice_manager = new VoiceManager();
	voice_dsp = new VoiceDsp();
}

VoiceManager* voice_manager=NULL;
