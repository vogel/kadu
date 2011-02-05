/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>

#include "../sound/sound-file.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "exports.h"

#include "dsp_sound.h"

/**
 * @ingroup dsp_sound
 * @{
 */
extern "C" KADU_EXPORT int dsp_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	oss_player_slots = new OSSPlayerSlots;
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/dsp_sound.ui"));

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void dsp_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/dsp_sound.ui"));
	delete oss_player_slots;
	oss_player_slots = 0;

	kdebugf2();
}

struct OSSSoundDevice
{
	int fd;
	int max_buf_size;
	bool flushing;
	int sample_rate;
	int channels;
};

int OSSPlayerSlots::write_all(int fd, const char *data, int length, int chunksize)
{
	int res = 0, written = 0;

	while (written < length) {
		int towrite = (chunksize < length - written) ? chunksize : length - written;
		res = write(fd, data + written, towrite);
		if (res == -1) {
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			if (errno == EAGAIN)
				continue;
			else
				break;
		} else {
			written += towrite;
			res = written;
		}
	}
	return res;
}

OSSPlayerSlots::OSSPlayerSlots(QObject *parent) : QObject(parent)
{
	kdebugf();

	createDefaultConfiguration();

	connect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice*)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice*)), Qt::DirectConnection);
	connect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	connect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const qint16 *, int, bool *)),
		this, SLOT(playSample(SoundDevice, const qint16 *, int, bool *)),
		Qt::DirectConnection);
	connect(sound_manager, SIGNAL(setFlushingEnabledImpl(SoundDevice, bool)),
		this, SLOT(setFlushingEnabled(SoundDevice, bool)));

	kdebugf2();
}

OSSPlayerSlots::~OSSPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice*)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice*)));
	disconnect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	disconnect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const qint16 *, int, bool *)),
		this, SLOT(playSample(SoundDevice, const qint16 *, int, bool *)));
	disconnect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, qint16 *, int, bool *)),
		this, SLOT(recordSample(SoundDevice, qint16 *, int, bool *)));
	disconnect(sound_manager, SIGNAL(setFlushingEnabledImpl(SoundDevice, bool)),
		this, SLOT(setFlushingEnabled(SoundDevice, bool)));

	kdebugf2();
}

void OSSPlayerSlots::openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice* device)
{
	kdebugf();
	int maxbufsize = 0, caps = 0, value;
	*device = NULL;

	QString sdev = config_file.readEntry("Sounds","OutputDevice", "/dev/dsp");
	kdebugm(KDEBUG_INFO, "Opening %s\n", qPrintable(sdev));

	int flags;
	if (type == SoundDevicePlayOnly)
		flags = O_WRONLY;
	else
		flags = O_RDWR;

	int fd = open(qPrintable(sdev), flags);
	if (fd < 0)
	{
		fprintf(stderr, "Error opening device (%s, %d)\n", strerror(errno), errno);
		return;
	}

	kdebugm(KDEBUG_INFO, "Resetting\n");
	if (ioctl(fd, SNDCTL_DSP_RESET, 0) < 0)
	{
		fprintf(stderr, "Error resetting (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}

	// WARNING: parameters FORMAT, CHANNELS, SPEED must be set in that order
	// because of compatibility with some (old) soundcards
	kdebugm(KDEBUG_INFO, "Setting format\n");
	value = AFMT_S16_LE;
	if (ioctl(fd, SNDCTL_DSP_SETFMT, &value) < 0)
	{
		fprintf(stderr, "Error setting format (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting channels\n");
	value = channels;
	if (ioctl(fd, SNDCTL_DSP_CHANNELS, &value) < 0)
	{
		fprintf(stderr, "Error setting channels (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting speed\n");
	value = sample_rate;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &value) < 0)
	{
		fprintf(stderr, "Error setting speed (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "getting buffer size\n");
	if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &maxbufsize) < 0)
	{
		fprintf(stderr, "Error getting max buffer size (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "checking capabilities\n");
	if (ioctl(fd, SNDCTL_DSP_GETCAPS, &caps) < 0)
	{
		fprintf(stderr, "Error getting capabilities (%s, %d)\n", strerror(errno), errno);
		close(fd);
		return;
	}
	else
	{
#if defined(DSP_CAP_MULTI) && defined(DSP_CAP_BIND)
		kdebugm(KDEBUG_INFO, "soundcard capabilities: rev=%d, duplex=%d, "
						"realtime=%d, batch=%d, coproc=%d, trigger=%d, "
						"mmap=%d, multi=%d, bind=%d\n",
			caps&DSP_CAP_REVISION, (caps&DSP_CAP_DUPLEX)!=0, (caps&DSP_CAP_REALTIME)!=0,
			(caps&DSP_CAP_BATCH)!=0, (caps&DSP_CAP_COPROC)!=0, (caps&DSP_CAP_TRIGGER)!=0,
			(caps&DSP_CAP_MMAP)!=0, (caps&DSP_CAP_MULTI)!=0, (caps&DSP_CAP_BIND)!=0);
#endif
	}


	kdebugm(KDEBUG_FUNCTION_END, "Setup successful, fd=%d maxbuf=%d\n", fd, maxbufsize);
	OSSSoundDevice* dev = new OSSSoundDevice;
	dev->fd = fd;
	dev->max_buf_size = maxbufsize;
	dev->sample_rate = sample_rate;
	dev->channels = channels;
	dev->flushing = false;
	*device = (SoundDevice) dev;
}

void OSSPlayerSlots::closeDevice(SoundDevice device)
{
	kdebugf();
	OSSSoundDevice* dev = (OSSSoundDevice*)device;
	if (!dev)
	{
		kdebugm(KDEBUG_WARNING, "cannot close device, device not opened\n");
		return;
	}
	if (dev->fd!=-1)
	{
		close(dev->fd);
		dev->fd = -1;
	}
	delete dev;
	device = NULL;
	kdebugf2();
}

void OSSPlayerSlots::playSample(SoundDevice device, const qint16 *data, int length, bool *result)
{
	kdebugf();

	*result = true;
	OSSSoundDevice* dev = (OSSSoundDevice*)device;
	if (!dev || dev->fd<0)
	{
		*result = false;
		kdebugm(KDEBUG_WARNING, "cannot play sample, device not opened, dev:%p dev->fd:%d\n", dev, dev?dev->fd:-1);
		return;
	}
	write_all(dev->fd, (char*)data, length, dev->max_buf_size);

	if (dev->flushing)
	{
		// wait for end of playing
		if (ioctl(dev->fd, SNDCTL_DSP_SYNC, 0) < 0)
		{
			fprintf(stderr, "SNDCTL_DSP_SYNC error (%s, %d)\n", strerror(errno), errno);
			*result = false;
		}
	}
	kdebugf2();
}

void OSSPlayerSlots::setFlushingEnabled(SoundDevice device, bool enabled)
{
	kdebugf();
	OSSSoundDevice* dev = (OSSSoundDevice*)device;
	if (dev)
		dev->flushing = enabled;
	kdebugf2();
}

int OSSPlayerSlots::read_all(int fd, char *buffer, int count)
{
	kdebugf();
	int offset = 0,c;
	while (offset<count)
	{
		c = read(fd, buffer + offset, count - offset);
		if (c == -1)
		{
			kdebugmf(KDEBUG_WARNING, "%s (%d)\n", strerror(errno), errno);
			return -1;
		}
//		else
//			kdebugm(KDEBUG_INFO, "read: %d\n", c);
		offset += c;
	}
	return offset;
}

void OSSPlayerSlots::createDefaultConfiguration()
{
	config_file.addVariable("Sounds", "OutputDevice", "/dev/dsp");
}

OSSPlayerSlots *oss_player_slots;

/** @} */

