/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <unistd.h>
#include <errno.h>

#include <esd.h>
#include "esd_sound.h"
#include "debug.h"
#include "../sound/sound.h"

/**
 * @ingroup esd_sound
 * @{
 */
extern "C" int esd_sound_init()
{
	kdebugf();
	esd_player = new ESDPlayer(NULL, "esd_player_slots");
	kdebugf2();
	return 0;
}
extern "C" void esd_sound_close()
{
	kdebugf();
	delete esd_player;
	esd_player = NULL;
	kdebugf2();
}

ESDPlayer::ESDPlayer(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

	connect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice&)),
			this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice&)));
	connect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
			this, SLOT(closeDevice(SoundDevice)));
	connect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
			this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	connect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
			this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));
	connect(sound_manager, SIGNAL(setFlushingEnabledImpl(SoundDevice, bool)),
			this, SLOT(setFlushingEnabled(SoundDevice, bool)));

	kdebugf2();
}

ESDPlayer::~ESDPlayer()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice&)),
			this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice&)));
	disconnect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
			this, SLOT(closeDevice(SoundDevice)));
	disconnect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
			this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	disconnect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
			this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));
	disconnect(sound_manager, SIGNAL(setFlushingEnabledImpl(SoundDevice, bool)),
			this, SLOT(setFlushingEnabled(SoundDevice, bool)));

	kdebugf2();
}

struct ESDDevice
{
	int play_sock, rec_sock;
	ESDDevice():play_sock(-1),rec_sock(-1){}
};

void ESDPlayer::openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device)
{
	kdebugf();
	ESDDevice *dev = new ESDDevice();
	device = (SoundDevice) dev;
	if (type == PLAY_ONLY || type == PLAY_AND_RECORD)
	{
		dev->play_sock = esd_play_stream(ESD_BITS16|(channels==2?ESD_STEREO:ESD_MONO), sample_rate, NULL, NULL);
		if (dev->play_sock<=0)
		{
			delete dev;
			device = NULL;
			return;
		}
	}
	if (type == RECORD_ONLY || type == PLAY_AND_RECORD)
	{
		dev->rec_sock = esd_record_stream(ESD_BITS16|(channels==2?ESD_STEREO:ESD_MONO), sample_rate, NULL, NULL);
		if (dev->rec_sock<=0)
		{
			esd_close(dev->play_sock);
			delete dev;
			device = NULL;
			return;
		}
	}
	kdebugf2();
}

void ESDPlayer::closeDevice(SoundDevice device)
{
	kdebugf();
	ESDDevice *dev = (ESDDevice *) device;
	if (!dev)
	{
		kdebugf2();
		return;
	}
	if (dev->play_sock>0)
		esd_close(dev->play_sock);
	if (dev->rec_sock>0)
		esd_close(dev->rec_sock);
	kdebugf2();
}

void ESDPlayer::playSample(SoundDevice device, const int16_t* data, int length, bool& result)
{
	kdebugf();
	ESDDevice *dev = (ESDDevice *) device;
	if (!dev)
	{
		kdebugf2();
		return;
	}

	int res = 0, written = 0;
	const char *cdata = (const char *)data;
	while (written < length)
	{
		int towrite = (ESD_BUF_SIZE < length - written) ? ESD_BUF_SIZE : length - written;
//		int towrite = length - written;
		res = write(dev->play_sock, cdata + written, towrite);
		if (res == -1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			if (errno == EAGAIN)
				continue;
			else
				break;
		}
		else
			written += res;
	}
	result = written == length;

	kdebugf2();
}

void ESDPlayer::recordSample(SoundDevice device, int16_t* data, int length, bool& result)
{
	kdebugf();
	ESDDevice *dev = (ESDDevice *) device;
	if (!dev)
	{
		kdebugf2();
		return;
	}

	int res = 0, reed = 0;
	char *cdata = (char *)data;
	while (reed < length)
	{
		int toread = (ESD_BUF_SIZE < length - reed) ? ESD_BUF_SIZE : length - reed;
//		int toread = length - reed;
		res = read(dev->rec_sock, cdata + reed, toread);
		if (res == -1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			if (errno == EAGAIN)
				continue;
			else
				break;
		}
		else
			reed += res;
	}
	result = reed == length;

	kdebugf2();
}

void ESDPlayer::setFlushingEnabled(SoundDevice device, bool /*enabled*/)
{
	kdebugf();
	ESDDevice *dev = (ESDDevice *) device;
	if (!dev)
	{
		kdebugf2();
		return;
	}
	// unimplemented
	kdebugf2();
}

ESDPlayer *esd_player;

/** @} */

