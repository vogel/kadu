/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>

#include "dsp_sound.h"
#include "debug.h"
#include "config_dialog.h"
#include "sound_desc.h"

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

struct DspSoundDevice
{
	int fd;
	int max_buf_size;
};

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
		thread->list.push_back(s);
		thread->list.push_back(t);
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

	kdebugm(KDEBUG_INFO, "Opening /dev/dsp\n");
	int fd = open("/dev/dsp", O_RDWR);
	if(fd<0)
	{
		kdebugm(KDEBUG_ERROR, "Error opening /dev/dsp\n");
		return;
	}

	if (ioctl(fd, SNDCTL_DSP_RESET)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error resetting /dev/dsp\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting speed for /dev/dsp\n");
	int value = sample_rate;
	if(ioctl(fd, SNDCTL_DSP_SPEED, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting speed for /dev/dsp\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting sample size for /dev/dsp\n");
	value = 16;
	if(ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting sample size for /dev/dsp\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting channels for /dev/dsp\n");
	value = channels;
	if(ioctl(fd, SNDCTL_DSP_CHANNELS, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting channels for /dev/dsp\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_INFO, "Setting ftm for /dev/dsp\n");
	value = AFMT_S16_LE;
	if(ioctl(fd, SNDCTL_DSP_SETFMT, &value)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error setting ftm for /dev/dsp\n");
		close(fd);
		return;
	}

	int maxbufsize;
	if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &maxbufsize)<0)
	{
		kdebugm(KDEBUG_ERROR, "Error getting max buffer size for /dev/dsp\n");
		close(fd);
		return;
	}

	kdebugm(KDEBUG_FUNCTION_END, "Setup successful, fd=%d\n", fd);
	DspSoundDevice* dev = new DspSoundDevice;
	dev->fd = fd;
	dev->max_buf_size = maxbufsize;
	device = dev;
}

void OSSPlayerSlots::closeDevice(SoundDevice device)
{
	kdebugf();
	DspSoundDevice* dev = (DspSoundDevice*)device;			
	close(dev->fd);
	delete dev;
	kdebugf2();
}

void OSSPlayerSlots::playSample(SoundDevice device, const int16_t* data, int length, bool& result)
{
	kdebugf();
	result = true;
	DspSoundDevice* dev = (DspSoundDevice*)device;			
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
	result = (read(((DspSoundDevice*)device)->fd, data, length) == length);
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
	QString path,device;
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
		path=list.first();		list.pop_front();
		device=list.first();	list.pop_front();

		sound_desc *sound=new sound_desc(path.ascii());
		if (sound->isOk())
		{
			int stereo;
			int fd=open(device.ascii(), O_WRONLY);
			if (fd<0)
			{
				qWarning("open: %s", strerror(errno));
				mutex.unlock();
				continue;
			}
			
			if (ioctl(fd, SNDCTL_DSP_RESET)<0)
			{
				qWarning("ioctl: %s", strerror(errno));
				close(fd);
				mutex.unlock();
				continue;
			}

			stereo=(sound->channels==2);

			if (ioctl(fd, SNDCTL_DSP_STEREO, &stereo)<0)
			{
				qWarning("ioctl: %s", strerror(errno));
				close(fd);
				mutex.unlock();
				continue;
			}

			if (ioctl(fd, SNDCTL_DSP_SPEED, &(sound->speed))<0)
			{
				qWarning("ioctl: %s", strerror(errno));
				close(fd);
				mutex.unlock();
				continue;
			}
			
			int unit=sound->unit;
			if (ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &unit)<0)
			{
				qWarning("ioctl: %s", strerror(errno));
				close(fd);
				mutex.unlock();
				continue;
			}

			int maxbufsize;
			if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &maxbufsize)<0)
			{
				qWarning("ioctl: %s", strerror(errno));
				close(fd);
				mutex.unlock();
				continue;
			}
			
			int c=0;
			int len=sound->length;
			while (c<len)
			{
				write(fd, sound->data+c, (maxbufsize<len-c)?maxbufsize:len-c);
				c+=maxbufsize;
			}
			close(fd);
		}//end if (sound->isOk())
		mutex.unlock();
		kdebugm(KDEBUG_INFO, "unlocked\n");
		delete sound;
	}//end while(!end)
	kdebugf2();
}

OSSPlayerSlots *oss_player_slots;

