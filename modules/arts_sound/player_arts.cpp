/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "player_arts.h"
#include "debug.h"
#include "../sound/sound_file.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <qprocess.h>
#include <qapplication.h>
#include "arts_connector/common.c"

aRtsDevice::aRtsDevice() : process(NULL), sock(-1), no(-1), valid(true)
{
}

void aRtsDevice::processExited()
{
	kdebugf();
	mutex.lock();
	kdebugmf(KDEBUG_INFO, "locked\n");
	if (sock != -1)
	{
		close(sock);
		sock = -1;
	}
	valid = false;
	kdebugmf(KDEBUG_INFO, "unlocking\n");
	mutex.unlock();
	kdebugf2();
}

aRtsDevice::~aRtsDevice()
{
	kdebugf();
}

void aRtsDevice::deleteLater2()
{
	kdebugf();
	if (sock != -1)
	{
		close(sock);
		sock = -1;
	}
	valid = false;

	if (process)
	{
		disconnect(process, SIGNAL(processExited()), this, SLOT(processExited()));
		process->tryTerminate();
		QTimer::singleShot(5000, process, SLOT(kill()));
		QTimer::singleShot(5500, process, SLOT(deleteLater()));
	}
	deleteLater();
	kdebugf2();
}

extern "C" int arts_sound_init()
{
	kdebugf();
	arts_player_recorder = new aRtsPlayerRecorder(NULL, "arts_player_recorder");
	kdebugf2();
	return 0;
}

extern "C" void arts_sound_close()
{
	kdebugf();
	delete arts_player_recorder;
	arts_player_recorder = NULL;
	kdebugf2();
}

aRtsPlayerRecorder::aRtsPlayerRecorder(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	num = 0;
	srandom(time(NULL));
	finalizing = false;
	
	connect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice&)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice&)));
	connect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	connect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	connect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));

	kdebugf2();
}

aRtsPlayerRecorder::~aRtsPlayerRecorder()
{
	kdebugf();
	aRtsDevice *dev;
	finalizing = true;

	// mordujemy wszystkie procesy, które zawis³y
	busymutex.lock();
	while (!busy.empty())
	{
		dev = busy.last();
		busy.pop_back();

		//przez chwilê nie potrzebujemy muteksa, a bêdzie on potrzebny przy zamykaniu urz±dzenia
		busymutex.unlock();
		dev->process->tryTerminate();
		QTimer::singleShot(5000, dev->process, SLOT(kill()));
		
		//czekamy a¿ w±tek korzystaj±cy z urz±dzenia go zwolni
		dev->inUse.lock();
		dev->inUse.unlock();
		busymutex.lock();
	}
	busymutex.unlock();

	disconnect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice&)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice&)));
	disconnect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	disconnect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool&)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool&)));
	disconnect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool&)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool&)));

	// a teraz te które s± w puli wolnych urz±dzeñ
	poolmutex.lock();
	while (!pool.empty())
	{
		dev = pool.last();
		pool.pop_back();
		dev->deleteLater2();
	}
	poolmutex.unlock();

	kdebugf2();
}

void aRtsPlayerRecorder::openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice &device)
{
	int itype;
	if (type == PLAY_ONLY)
		itype = 1;
	else if (type == RECORD_ONLY)
		itype = 2;
	else // PLAY_AND_RECORD
		itype = 3;
	kdebugmf(KDEBUG_FUNCTION_START, "rate:%d channels:%d type:%d\n", sample_rate, channels, itype);

	struct sockaddr_un unix_sock_name;
	char tmp[100];
	int cerr, sock;
	
	sock = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (sock == -1)
	{
		kdebugm(KDEBUG_ERROR, "sock()==-1 %d %s\n", errno, strerror(errno));
		device = NULL;
		kdebugf2();
		return;
	}

	aRtsDevice *dev;
	poolmutex.lock();
	if (pool.empty())
	{
		poolmutex.unlock();
		++num;
		dev = new aRtsDevice();
		dev->mutex.lock();
		long pass = random();
//		kdebugm(KDEBUG_INFO, "creating process\n");
		dev->process = new QProcess(dataPath("kadu/modules/bin/arts_sound/arts_connector"));
//		kdebugm(KDEBUG_INFO, "connecting processExited() signal\n");
		connect(dev->process, SIGNAL(processExited()), dev, SLOT(processExited()));
		kdebugm(KDEBUG_INFO, "starting process\n");
		if (!dev->process->start())
		{
			disconnect(dev->process, SIGNAL(processExited()), dev, SLOT(processExited()));
			delete dev->process;
			dev->mutex.unlock();
			delete dev;
			device = NULL;
			kdebugf2();
			return;
		}
		kdebugm(KDEBUG_INFO, "writing to stdin\n");
		dev->process->writeToStdin(QString("%1 %2 %3\n").arg(config_file.readNumEntry("General", "UIN")).arg(pass).arg(num));
		//UWAGA: writeToStdin dostarcza dane w pêtli zdarzeñ Qt

		kdebugm(KDEBUG_INFO, "waiting for new line from arts_connector\n");
		while (dev->valid && !dev->process->canReadLineStdout())
		{
			//dajemy szansê zakoñczyæ siê procesowi (processExited musi zablokowaæ urz±dzenie)
//			kdebugm(KDEBUG_INFO, "releasing lock\n");
			dev->mutex.unlock();
			usleep(100000);//0.1 s
//			kdebugm(KDEBUG_INFO, "locking\n");
			qApp->processEvents();
			dev->mutex.lock();
//			kdebugm(KDEBUG_INFO, "locked again\n");
		}
		kdebugm(KDEBUG_INFO, "process exited or new line read\n");
		QString out;
		if (dev->valid)
			out = dev->process->readLineStdout();
		kdebugm(KDEBUG_INFO, "%d, process returned: '%s'\n", dev->valid, out.local8Bit().data());

		if (dev->valid && dev->process->canReadLineStderr())
			kdebugm(KDEBUG_WARNING, "process written on stderr: %s\n", dev->process->readLineStderr().local8Bit().data());
		if (out != "OK" || !dev->valid)
		{
			dev->mutex.unlock();
			dev->deleteLater2();
			device = NULL;
			kdebugf2();
			return;
		}
	
		unix_sock_name.sun_family = AF_LOCAL;

		sprintf(tmp, "/tmp/kadu-arts-connector-%d-%d", config_file.readNumEntry("General", "UIN"), num);
		strncpy(unix_sock_name.sun_path, tmp, sizeof(unix_sock_name.sun_path));
		unix_sock_name.sun_path[sizeof(unix_sock_name.sun_path)-1] = 0;	
		cerr = ::connect(sock, (struct sockaddr *)&unix_sock_name, SUN_LEN(&unix_sock_name));
		if (cerr == -1)
		{
			kdebugm(KDEBUG_ERROR, "connect(%d)==-1 %d %s\n", sock, errno, strerror(errno));

			dev->mutex.unlock();
			dev->deleteLater2();
			device = NULL;
			kdebugf2();
			return;
		}
		dev->sock = sock;
		sprintf(tmp, "PASS %ld\n", pass);
		kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
		dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 100) != -1;
	}
	else
	{
		dev = pool.last();
		pool.pop_back();
		poolmutex.unlock();
		dev->mutex.lock();
		kdebugm(KDEBUG_INFO, "%p %d\n", dev->process, dev->sock);
	}
	device = dev;

	sprintf(tmp, "OPEN %d %d %d\n", sample_rate, channels, itype);
	kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
	dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 100) != -1;
	dev->valid = dev->valid && read_line(dev->sock, tmp, 100) != -1;
	kdebugm(KDEBUG_INFO, "%d, ret: '%s'\n", dev->valid, dev->valid ? tmp : "");
	if (!dev->valid || sscanf(tmp, "OPENED %d", &dev->no) != 1 || dev->no < 0)
	{
		sprintf(tmp, "QUIT\n");
		kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
		dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 100) != -1;
		dev->mutex.unlock();
		dev->deleteLater2();
		device = NULL;
		kdebugf2();
		return;
	}
	dev->mutex.unlock();
	dev->inUse.lock();
	busymutex.lock();
	busy.push_back(dev);
	busymutex.unlock();

	kdebugf2();
}

void aRtsPlayerRecorder::closeDevice(SoundDevice device)
{
	kdebugf();
	aRtsDevice *dev = (aRtsDevice*)device;
	if (!dev)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "null\n");
		return;
	}
	dev->mutex.lock();

	char tmp[50];
	sprintf(tmp, "CLOSE %d\n", dev->no);
	kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
	dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 50) != -1;
	dev->valid = dev->valid && read_line(dev->sock, tmp, 50) != -1;

	poolmutex.lock();
	if (!finalizing && (!dev->valid || pool.size() > 2))
	{
		poolmutex.unlock();
		dev->mutex.unlock();
		dev->inUse.unlock();
		busymutex.lock();
		busy.remove(dev);
		busymutex.unlock();

		dev->deleteLater2();
	}
	else
	{
		dev->mutex.unlock();
		dev->inUse.unlock();
		pool.push_back(dev);
		poolmutex.unlock();

		busymutex.lock();
		busy.remove(dev);
		busymutex.unlock();
	}
	kdebugf2();
}

void aRtsPlayerRecorder::playSample(SoundDevice device, const int16_t* data, int length, bool& result)
{
	kdebugf();
	aRtsDevice *dev = (aRtsDevice*)device;
	if (!dev)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "null\n");
		return;
	}
	dev->mutex.lock();
	char tmp[50];
	int success;
	sprintf(tmp, "PLAY %d %d\n", dev->no, length);
	kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
	dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 50) != -1;
	dev->valid = dev->valid && write_all(dev->sock, (const char *)data, length, 65536) != -1;
	dev->valid = dev->valid && read_line(dev->sock, tmp, 50) != -1;
	kdebugm(KDEBUG_INFO, "%d, ret: '%s'\n", dev->valid, dev->valid ? tmp : "");
	if (!dev->valid || sscanf(tmp, "PLAY SUCCESS: %d", &success) != 1)
		result = false;
	else
		result = success;
	dev->mutex.unlock();
//	sleep(1);
	kdebugf2();
}

void aRtsPlayerRecorder::recordSample(SoundDevice device, int16_t* data, int length, bool& result)
{
	kdebugf();
	aRtsDevice *dev = (aRtsDevice*)device;
	if (!dev)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "null\n");
		return;
	}
	dev->mutex.lock();
	char tmp[50];
	int success;
	sprintf(tmp, "RECORD %d %d\n", dev->no, length);
	kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
	dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 50) != -1;
	dev->valid = dev->valid && read_all(dev->sock, (char *)data, length) != -1;
	dev->valid = dev->valid && read_line(dev->sock, tmp, 50) != -1;
	kdebugm(KDEBUG_INFO, "%d, ret: '%s'\n", dev->valid, dev->valid ? tmp : "");
	if (!dev->valid || sscanf(tmp, "RECORD SUCCESS: %d", &success) != 1)
		result = false;
	else
		result = success;
	dev->mutex.unlock();
	kdebugf2();
}

void aRtsPlayerRecorder::setFlushingEnabled(SoundDevice device, bool enabled)
{
	kdebugf();
	char tmp[50];
	aRtsDevice *dev = (aRtsDevice*)device;
	if (!dev)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "null\n");
		return;
	}
	dev->mutex.lock();
	sprintf(tmp, "SETFLUSHING %d %d\n", dev->no, (int)enabled);
	kdebugm(KDEBUG_INFO, "%d, sending: '%s'\n", dev->valid, tmp);
	dev->valid = dev->valid && write_all(dev->sock, tmp, strlen(tmp), 50) != -1;
	dev->valid = dev->valid && read_line(dev->sock, tmp, 50) != -1;
	kdebugm(KDEBUG_INFO, "%d, ret: '%s'\n", dev->valid, dev->valid ? tmp : "");
	dev->mutex.unlock();
	kdebugf2();
}

aRtsPlayerRecorder *arts_player_recorder;
