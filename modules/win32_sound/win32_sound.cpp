/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <string.h>

#include <windows.h>

typedef short int16_t;

#include "configuration/configuration-window-widgets.h"
#include "../sound/sound_file.h"
#include "debug.h"
#include "win32_sound.h"

/**
 * @ingroup dsp_sound
 * @{
 */
extern "C" KADU_EXPORT int win32_sound_init(bool firstLoad)
{
	kdebugf();

	win32_player_slots = new WIN32PlayerSlots(NULL, "win32_player_slots");
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/win32_sound.ui"), win32_player_slots);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void win32_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/win32_sound.ui"), 0);
	delete win32_player_slots;
	win32_player_slots = 0;

	kdebugf2();
}

struct WIN32SoundDevice
{
	HWAVEOUT out;
	HWAVEIN in;
};


WIN32PlayerSlots::WIN32PlayerSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();

	createDefaultConfiguration();

	connect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice*)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice*)), Qt::DirectConnection);
	connect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	connect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool*)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool*)),
		Qt::DirectConnection);
	connect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool*)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool*)),
		Qt::DirectConnection);

  inNames.clear();
  inValues.clear();
	inNames.append(tr("Default"));
	inValues.append("-1");
	int inputDevices = waveInGetNumDevs();
	for(int i = 0; i < inputDevices; i++) {
		QT_WA( {
			WAVEINCAPSW caps;
			if(waveInGetDevCapsW(i, &caps, sizeof(WAVEINCAPSW)) == MMSYSERR_NOERROR){
				inNames.append(QString::fromUtf16(caps.szPname));
				inValues.append(QString::number(i));
			}
		} , {
			WAVEINCAPSA caps;
			if(waveInGetDevCapsA(i, &caps, sizeof(WAVEINCAPSW)) == MMSYSERR_NOERROR){
				inNames.append(QString::fromLocal8Bit(caps.szPname));
				inValues.append(QString::number(i));
			}
		} );
	}

  outNames.clear();
  outValues.clear();
	outNames.append(tr("Default"));
	outValues.append("-1");
	int outputDevices = waveOutGetNumDevs();
	for(int i  = 0;i < outputDevices; i++) {
		QT_WA( {
			WAVEOUTCAPSW caps;
			if(waveOutGetDevCapsW(i, &caps, sizeof(WAVEOUTCAPSW)) == MMSYSERR_NOERROR){
				outNames.append(QString::fromUtf16(caps.szPname));
				outValues.append(QString::number(i));
			}
		} , {
			WAVEOUTCAPSA caps;
			if(waveOutGetDevCapsA(i, &caps, sizeof(WAVEOUTCAPSA)) == MMSYSERR_NOERROR){
				outNames.append(QString::fromLocal8Bit(caps.szPname));
				outValues.append(QString::number(i));
			}
		} );
	}
	
	kdebugf2();
}

WIN32PlayerSlots::~WIN32PlayerSlots()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(openDeviceImpl(SoundDeviceType, int, int, SoundDevice*)),
		this, SLOT(openDevice(SoundDeviceType, int, int, SoundDevice*)));
	disconnect(sound_manager, SIGNAL(closeDeviceImpl(SoundDevice)),
		this, SLOT(closeDevice(SoundDevice)));
	disconnect(sound_manager, SIGNAL(playSampleImpl(SoundDevice, const int16_t*, int, bool*)),
		this, SLOT(playSample(SoundDevice, const int16_t*, int, bool*)));
	disconnect(sound_manager, SIGNAL(recordSampleImpl(SoundDevice, int16_t*, int, bool*)),
		this, SLOT(recordSample(SoundDevice, int16_t*, int, bool*)));

	kdebugf2();
}

void WIN32PlayerSlots::openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice* device)
{
	kdebugf();
	WIN32SoundDevice *dev=new WIN32SoundDevice;
	MMRESULT res;

	WAVEFORMATEX f;
	f.wFormatTag=WAVE_FORMAT_PCM;
	f.nChannels=channels;
	f.nSamplesPerSec=sample_rate;
	f.nBlockAlign=channels*2;
	f.nAvgBytesPerSec=sample_rate*f.nBlockAlign;
	f.wBitsPerSample=16;
	f.cbSize=0;

	dev->in=0;
	dev->out=0;

	if(type==PLAY_ONLY || type==PLAY_AND_RECORD){
		res=waveOutOpen(&dev->out, config_file.readNumEntry("Sounds", "WaveOut"), &f, NULL, NULL, CALLBACK_NULL);
		if(res!=MMSYSERR_NOERROR){
			char buff[256];
			waveOutGetErrorText(res, buff, 255);
			kdebugm(KDEBUG_ERROR, "Unable to open playback sound device: %s\n", buff);
			return;
		}
	}

	if(type==RECORD_ONLY || type==PLAY_AND_RECORD){
		res=waveInOpen(&dev->in, config_file.readNumEntry("Sounds", "WaveIn"), &f, NULL, NULL, CALLBACK_NULL);
		if(res!=MMSYSERR_NOERROR){
			char buff[256];
			waveInGetErrorText(res, buff, 255);
			kdebugm(KDEBUG_ERROR, "Unable to open recording sound device: %s\n", buff);
			return;
		}
	}

	*device=(SoundDevice)dev;
}

void WIN32PlayerSlots::closeDevice(SoundDevice device)
{
	kdebugf();
	WIN32SoundDevice* dev=(WIN32SoundDevice*)device;
	if(!dev){
		kdebugm(KDEBUG_ERROR, "Device not opened");
		return;
	}

	if(dev->out)
		waveOutClose(dev->out);

	if(dev->in)
		waveInClose(dev->in);

	delete dev;
	device=NULL;

	kdebugf2();
}

void WIN32PlayerSlots::playSample(SoundDevice device, const int16_t* data, int length, bool* result)
{
	kdebugf();
	WAVEHDR hdr;
	WIN32SoundDevice *dev=(WIN32SoundDevice*)device;

	if(!dev){
		kdebugm(KDEBUG_ERROR, "device not openend\n");
		*result=false;
		return;
	}

	*result=true;

	hdr.lpData=(LPSTR)data;
	hdr.dwBufferLength=length;
	hdr.dwFlags=0;
	hdr.dwLoops=0;
	waveOutPrepareHeader(dev->out, &hdr, sizeof(WAVEHDR));
	MMRESULT res=waveOutWrite(dev->out, &hdr, sizeof(WAVEHDR));
	if(res!=MMSYSERR_NOERROR){
		char buff[256];
		waveOutGetErrorText(res, buff, 255);
		kdebugm(KDEBUG_ERROR, "Unable to play waveform data: %s\n", buff);
	}
	while(!(hdr.dwFlags&WHDR_DONE)) {};
	waveOutUnprepareHeader(dev->out, &hdr, sizeof(WAVEHDR));
	kdebugf2();
}

void WIN32PlayerSlots::recordSample(SoundDevice device, int16_t* data, int length, bool* result)
{
	kdebugf();
	WAVEHDR hdr;
	WIN32SoundDevice *dev=(WIN32SoundDevice*)device;

	if(!dev){
		kdebugm(KDEBUG_ERROR, "device not openend\n");
		*result=false;
		return;
	}

	hdr.lpData=(LPSTR)data;
	hdr.dwBufferLength=length;
	hdr.dwBytesRecorded=0;
	hdr.dwFlags=0;
	hdr.dwLoops=0;
	waveInPrepareHeader(dev->in, &hdr, sizeof(WAVEHDR));
	MMRESULT res=waveInAddBuffer(dev->in, &hdr, sizeof(WAVEHDR));
	if(res!=MMSYSERR_NOERROR){
		char buff[256];
		waveInGetErrorText(res, buff, 255);
		kdebugm(KDEBUG_ERROR, "Unable to play waveform data: %s\n", buff);
	}
	while (waveInUnprepareHeader(dev->in, &hdr, sizeof(WAVEHDR))==WAVERR_STILLPLAYING) {};
	kdebugf2();
}

void WIN32PlayerSlots::createDefaultConfiguration()
{
	// default to WAVE_MAPPER
	config_file.addVariable("Sounds", "WaveIn", -1);
	config_file.addVariable("Sounds", "WaveOut", -1);
}

void WIN32PlayerSlots::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigComboBox *in=dynamic_cast<ConfigComboBox*>(mainConfigurationWindow->widgetById("win32sound/in"));
	ConfigComboBox *out=dynamic_cast<ConfigComboBox*>(mainConfigurationWindow->widgetById("win32sound/out"));
	in->setItems(inValues, inNames);
	out->setItems(outValues, outNames);
}


WIN32PlayerSlots *win32_player_slots;

/** @} */
