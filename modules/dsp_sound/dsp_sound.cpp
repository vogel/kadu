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
#include "../sound/sound.h"
#include "sound_desc.h"

extern "C" int dsp_sound_init()
{
	kdebugf();
		
	directPlayerObj=new DirectPlayerSlots();

	QObject::connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
					 directPlayerObj, SLOT(playSound(const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
					 directPlayerObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
					 directPlayerObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::connect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
					 directPlayerObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));

	ConfigDialog::addHGroupBox("Sounds", "Sounds",
			QT_TRANSLATE_NOOP("@default","Output device"));
	ConfigDialog::addLineEdit("Sounds", "Output device", 
			QT_TRANSLATE_NOOP("@default","Path:"), "OutputDevice","/dev/dsp","","device_path");

	return 0;
}
extern "C" void dsp_sound_close()
{
	kdebugf();

	QObject::disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
						directPlayerObj, SLOT(playSound(const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnMessage(UinsList, const QString &, const QString &, bool, double)),
						directPlayerObj, SLOT(playMessage(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnChat(UinsList, const QString &, const QString &, bool, double)),
						directPlayerObj, SLOT(playChat(UinsList, const QString &, const QString &, bool, double)));
	QObject::disconnect(sound_manager, SIGNAL(playOnNotify(const uin_t, const QString &, bool, double)),
						directPlayerObj, SLOT(playNotify(const uin_t, const QString &, bool, double)));
	ConfigDialog::removeControl("Sounds", "Path:", "device_path");
	ConfigDialog::removeControl("Sounds", "Output device");
	delete directPlayerObj;
	directPlayerObj=NULL;
}

DirectPlayerSlots::DirectPlayerSlots():thread(NULL)
{
	kdebugf();
	error=true;
	thread=new DirectPlayThread();
	if (!thread)
		return;
	thread->start();
	error=false;
}

DirectPlayerSlots::~DirectPlayerSlots()
{
	kdebugf();
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
}

void DirectPlayerSlots::play(const QString &s, bool volCntrl, double vol, const QString &device)
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
}

void DirectPlayerSlots::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	QString dev=QString::null;
	if (ConfigDialog::dialogOpened())
	{
		QLineEdit *e_sounddev= ConfigDialog::getLineEdit("Sounds", "Path:", "device_path");
		dev=e_sounddev->text();
	}
	play(s, volCntrl, vol, dev);
}

void DirectPlayerSlots::playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void DirectPlayerSlots::playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

void DirectPlayerSlots::playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol)
{
	kdebugf();
	play(sound, volCntrl, vol, QString::null);
}

DirectPlayThread::DirectPlayThread()
{
	semaphore=new QSemaphore(100);
	(*semaphore)+=100;
}

DirectPlayThread::~DirectPlayThread()
{
	delete semaphore;
}

void DirectPlayThread::run()
{
	kdebugf();
	QString path,device;
	end=false;
	while (!end)
	{
		(*semaphore)++;
		mutex.lock();
		kdebug("locked\n");
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
		kdebug("unlocked\n");
	}//end while(!end)
}

DirectPlayerSlots *directPlayerObj;

