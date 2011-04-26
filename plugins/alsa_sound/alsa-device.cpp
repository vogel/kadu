/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdio.h>

#include "debug.h"

#include "alsa-device.h"

AlsaDevice::AlsaDevice(const QString &deviceName, int sampleRate, int channels) :
		Device(0), DeviceName(deviceName), SampleRate(sampleRate), Channels(channels)
{
}

AlsaDevice::~AlsaDevice()
{
}

snd_pcm_t * AlsaDevice::openDevice()
{
	kdebugf();

	bool play = true;

	snd_pcm_t *alsa_dev;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_uframes_t buffer_size, start_threshold;
	snd_pcm_uframes_t alsa_period_size, alsa_buffer_frames;
	snd_pcm_sw_params_t *sw_params;

	int err;

	alsa_period_size = 512;
	alsa_buffer_frames = 3 * alsa_period_size;

	// we need non-blocking behaviour only while opening device
	if ((err = snd_pcm_open(&alsa_dev, qPrintable(DeviceName), play ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
	{

		fprintf(stderr, "cannot open audio device \"%s\" (%s)\n", qPrintable(DeviceName), snd_strerror(err));
		fflush(stderr);
		return NULL;
	}

	// so after open we are immediately setting device to blocking mode
	snd_pcm_nonblock(alsa_dev, 0);
	kdebugm(KDEBUG_INFO, "device opened\n");

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "struct allocated\n");

	if ((err = snd_pcm_hw_params_any(alsa_dev, hw_params)) < 0)
	{
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "struct initialized\n");

	if ((err = snd_pcm_hw_params_set_access(alsa_dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "access type set\n");

	if ((err = snd_pcm_hw_params_set_format(alsa_dev, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "sample format set\n");

	if ((err = snd_pcm_hw_params_set_rate_near(alsa_dev, hw_params, (unsigned int*)&SampleRate, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "sample rate set\n");

	if ((err = snd_pcm_hw_params_set_channels(alsa_dev, hw_params, Channels)) < 0)
	{
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "channel count set\n");

	if ((err = snd_pcm_hw_params_set_buffer_size_near(alsa_dev, hw_params, &alsa_buffer_frames)) < 0)
	{
		fprintf(stderr, "cannot set buffer size (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "buffer size set\n");

	if ((err = snd_pcm_hw_params_set_period_size_near(alsa_dev, hw_params, &alsa_period_size, 0)) < 0)
	{
		fprintf(stderr, "cannot set period size (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "period size set\n");

	if ((err = snd_pcm_hw_params(alsa_dev, hw_params)) < 0)
	{
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "parameters set\n");

	/* extra check: if we have only one period, this code won't work */
	snd_pcm_hw_params_get_period_size(hw_params, &alsa_period_size, 0);
	snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
	if (alsa_period_size == buffer_size)
	{
		fprintf(stderr, "Can't use period equal to buffer size (%lu == %lu)\n", alsa_period_size, buffer_size);
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "period correctly set\n");


	snd_pcm_hw_params_free (hw_params);
	kdebugm(KDEBUG_INFO, "struct freed\n");

	if ((err = snd_pcm_sw_params_malloc(&sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params_malloc: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "another struct allocated\n");

	if ((err = snd_pcm_sw_params_current(alsa_dev, sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params_current: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "params got\n");

	/* round up to closest transfer boundary */
	start_threshold = 1;
	if ((err = snd_pcm_sw_params_set_start_threshold(alsa_dev, sw_params, start_threshold)) < 0)
	{
		fprintf(stderr, "cannot set start threshold (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "threshold set\n");

	if ((err = snd_pcm_sw_params(alsa_dev, sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "params ok\n");

	snd_pcm_sw_params_free (sw_params);
	kdebugm(KDEBUG_INFO, "struct freed\n");

	snd_pcm_reset(alsa_dev);

	kdebugf2();
	return alsa_dev;
}

bool AlsaDevice::open()
{
	Device = openDevice();
	return 0 != Device;
}

bool AlsaDevice::close()
{
	if (!Device)
		return false;

	snd_pcm_close(Device);
// 	delete Device;

	return true;
}

int AlsaDevice::xrunRecovery(int err)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: err:%d -EPIPE:%d -ESTRPIPE:%d\n", err, -EPIPE, -ESTRPIPE);
	if (err == -EPIPE)
	{
		err = snd_pcm_prepare(Device);
		if (err < 0)
		{
			kdebugm(KDEBUG_WARNING, "can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		}
		kdebugf2();
		return 0;
	}
	else if (err == -ESTRPIPE)
	{
		while ((err = snd_pcm_resume(Device)) == -EAGAIN)
			sleep(1);   /* wait until the suspend flag is released */
		if (err < 0)
		{
			err = snd_pcm_prepare(Device);
			if (err < 0)
			{
				kdebugm(KDEBUG_WARNING, "can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
			}
		}
		kdebugf2();
		return 0;
	}
	kdebugf2();
	return err;
}

bool AlsaDevice::playSample(short int *sampleData, int length)
{
	kdebugf();

	if (!Device)
		return false;

	int res, written = 0;
	int availErrorsCount = 0;

	while (written < length)
	{
		if ((res = snd_pcm_wait(Device, 100)) < 0)
			xrunRecovery(res);

		kdebugm(KDEBUG_DUMP, "snd_pcm_wait(player): %d\n", res);

		int towrite = (length - written) / ((sizeof(short int)) * Channels);
		int avail = snd_pcm_avail_update(Device);
		kdebugm(KDEBUG_DUMP, "snd_pcm_avail_update(player): %d\n", avail);

		if (avail < 0)
		{
			xrunRecovery(avail);
			avail = snd_pcm_avail_update(Device);
			kdebugm(KDEBUG_DUMP, "snd_pcm_avail_update(player): %d\n", avail);
		}

		if (avail <= 0)
		{
			kdebugm(KDEBUG_WARNING, "player avail error: %d\n", avail);
			++availErrorsCount;
			avail = 0;
		}
		else
			availErrorsCount = 0;

		if (availErrorsCount > 10)
			return false;

		if (avail < towrite)
			towrite = avail;

		kdebugm(KDEBUG_DUMP, "playing %d frames, bytes already played: %d\n", towrite, written);

		res = snd_pcm_writei(Device, sampleData + written, towrite);
		kdebugm(KDEBUG_DUMP, "played: %d\n", res);

		if (res == -EAGAIN || res == -EINVAL)
			// don't know why it is needed when we get EINVAL, but it works...
			continue;

		if (res == 0)
		{
			fprintf(stderr, "invalid sample length\n");
			fflush(stderr);
			return false;
		}

		if (res < 0)
		{
			if (xrunRecovery(res) < 0)
			{
				fprintf(stderr, "alsa write error: %s\n", snd_strerror(res));
				fflush(stderr);
				return false;
			}
		}
		else
			written += res * (sizeof(short int)) * Channels;
	}

	return true;
}
