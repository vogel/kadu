/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>

#include "../sound/sound_file.h"
#include "debug.h"
#include "dsp_sound.h"
#include "config_dialog.h"

extern "C" int dsp_sound_init()
{
	kdebugf();
		
	oss_player_slots=new OSSPlayerSlots(NULL, "oss_player_slots");

	ConfigDialog::addHGroupBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Output device"));
	ConfigDialog::addLineEdit("Sounds", "Output device", 
			QT_TRANSLATE_NOOP("@default","Path:"), "OutputDevice","/dev/dsp","","device_path");
	kdebugf2();
	return 0;
}

extern "C" void dsp_sound_close()
{
	kdebugf();

	ConfigDialog::removeControl("Sounds", "Path:", "device_path");
	ConfigDialog::removeControl("Sounds", "Output device");
	delete oss_player_slots;
	oss_player_slots=NULL;
	kdebugf2();
}

struct OSSSoundDevice
{
	int fd;
	int max_buf_size;
};

int OSSPlayThread::write_all(int fd, const char *data, int length, int chunksize)
{
	int res = 0, written = 0;
	
	while (written < length) {
		int towrite = (chunksize < length - written) ? chunksize : length - written;
		res = write(fd, data + written, towrite);
		if (res == -1) {
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

bool OSSPlayThread::play(const char *path, const char *device, bool volumeControl, float volume)
{
	bool ret=false;
	SoundFile *sound=new SoundFile(path);
	
	if (!sound->isOk())
	{
		kdebugmf(KDEBUG_ERROR, "sound is not ok?\n");
		delete sound;
		return false;
	}

	kdebugm(KDEBUG_INFO, "\n");
	kdebugm(KDEBUG_INFO, "length:   %d\n", sound->length);
	kdebugm(KDEBUG_INFO, "speed:    %d\n", sound->speed);
	kdebugm(KDEBUG_INFO, "channels: %d\n", sound->channels);

	if (volumeControl)
		sound->setVolume(volume);

	SoundDevice dev;
	oss_player_slots->openDevice(sound->speed, sound->channels, dev);

	if (((OSSSoundDevice*)dev)->fd>0)
		oss_player_slots->playSample(dev, sound->data, sound->length*sizeof(sound->data[0]), ret);
	
	oss_player_slots->closeDevice(dev);
	
	delete sound;
	return ret;
}


OSSPlayerSlots::OSSPlayerSlots(QObject *parent, const char *name) : QObject(parent, name), thread(NULL)
{
	kdebugf();
	error=true;
	thread=new OSSPlayThread();
	if (!thread)
		return;
	thread->start();
	error=false;

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
		this, SLOT(playSound(const QString &, bool, double)));
	connect(sound_manager, SIGNAL(openDeviceImpl(int, int, SoundDevice&)),
		this, SLOT(openDevice(int, int, SoundDevice&)));
	connect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	connect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	connect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));
	kdebugf2();
}

OSSPlayerSlots::~OSSPlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));
	disconnect(sound_manager, SIGNAL(openDeviceImpl(int, int, SoundDevice&)),
		this, SLOT(openDevice(int, int, SoundDevice&)));
	disconnect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	disconnect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	disconnect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));

	if (thread)
	{
		thread->mutex.lock();
		thread->end=true;
		thread->mutex.unlock();
		(*(thread->semaphore))--;
		thread->wait();
		delete thread;
		thread=NULL;
	}
	kdebugf2();
}

void OSSPlayerSlots::play(const QString &s, bool volCntrl, double vol, const QString &device)
{
	kdebugf();
	QString t;

	if (device==QString::null)
		t=config_file.readEntry("Sounds","OutputDevice", "/dev/dsp");
	else
		t=device;

	if (thread->mutex.tryLock())
	{
		thread->list.push_back(SndParams(s,t,volCntrl,vol));
		thread->mutex.unlock();
		(*(thread->semaphore))--;
	}
	kdebugf2();
}

void OSSPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString dev=QString::null;
	if (ConfigDialog::dialogOpened())
	{
		QLineEdit *e_sounddev= ConfigDialog::getLineEdit("Sounds", "Path:", "device_path");
		dev=e_sounddev->text();
	}
	play(s, volCntrl, vol, dev);
	kdebugf2();
}

void OSSPlayerSlots::openDevice(int sample_rate, int channels, SoundDevice& device)
{
	kdebugf();
	device = NULL;

	QString sdev=config_file.readEntry("Sounds","OutputDevice", "/dev/dsp");
	kdebugm(KDEBUG_INFO, "Opening %s\n", sdev.local8Bit().data());

	int fd = open(sdev.local8Bit().data(), O_RDWR);
	if (fd<0)
	{
		kdebugm(KDEBUG_ERROR, "Error opening device\n");
		return;
	}

	kdebugm(KDEBUG_INFO, "Resetting\n");
	if (ioctl(fd, SNDCTL_DSP_RESET)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error resetting\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting speed\n");
	int value = sample_rate;
	if(ioctl(fd, SNDCTL_DSP_SPEED, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting speed\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting channels\n");
	value = channels;
	if(ioctl(fd, SNDCTL_DSP_CHANNELS, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting channels\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting format\n");
	value = AFMT_S16_LE;
	if(ioctl(fd, SNDCTL_DSP_SETFMT, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting format\n");
		close(fd);
		return;
	}

	int maxbufsize;
	if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &maxbufsize)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error getting max buffer size\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_FUNCTION_END, "Setup successful, fd=%d\n", fd);
	OSSSoundDevice* dev = new OSSSoundDevice;
	dev->fd = fd;
	dev->max_buf_size = maxbufsize;
	device = (SoundDevice) dev;
}

void OSSPlayerSlots::closeDevice(SoundDevice device)
{
	kdebugf();
	OSSSoundDevice* dev = (OSSSoundDevice*)device;			
	close(dev->fd);
	delete dev;
	device = NULL;
	kdebugf2();
}

void OSSPlayerSlots::playSample(SoundDevice device, const int16_t* data, int length, bool& result)
{
	kdebugf();
	result = true;
	OSSSoundDevice* dev = (OSSSoundDevice*)device;
	int c = 0;
	while (c < length)
	{
		int l = (dev->max_buf_size < length - c) ? dev->max_buf_size : length - c;
		if (write(dev->fd, ((char*)data)+c, l) != l)
		{
			result = false;
			break;
		}
		c += dev->max_buf_size;
	}
	kdebugf2();
}

void OSSPlayerSlots::recordSample(SoundDevice device, int16_t* data, int length, bool& result)
{
	kdebugf();
	result = (read(((OSSSoundDevice*)device)->fd, data, length) == length);
	kdebugf2();
}

OSSPlayThread::OSSPlayThread()
{
	semaphore=new QSemaphore(100);
	(*semaphore)+=100;
}

OSSPlayThread::~OSSPlayThread()
{
	delete semaphore;
}

void OSSPlayThread::run()
{
	kdebugf();
	end=false;
	while (!end)
	{
		(*semaphore)++;
		mutex.lock();
		kdebugm(KDEBUG_INFO, "locked\n");
		if (end)
		{
			mutex.unlock();
			break;
		}
		SndParams params=list.first();
		list.pop_front();
		
		play(params.filename.local8Bit().data(), params.device.local8Bit().data(),
				params.volumeControl, params.volume);
		mutex.unlock();
		kdebugm(KDEBUG_INFO, "unlocked\n");
	}//end while(!end)
	kdebugf2();
}

SndParams::SndParams(QString fm, QString dev, bool volCntrl, float vol) :
			filename(fm), device(dev), volumeControl(volCntrl), volume(vol)
{
}

SndParams::SndParams(const SndParams &p) : filename(p.filename), device(p.device),
									volumeControl(p.volumeControl), volume(p.volume)
{
}

SndParams::SndParams()
{
}

OSSPlayerSlots *oss_player_slots;
