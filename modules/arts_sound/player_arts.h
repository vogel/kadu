#ifndef KADU_ARTS_H
#define KADU_ARTS_H
#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qmutex.h>
#include "../sound/sound.h"

class QProcess;

class aRtsDevice : public QObject
{
	Q_OBJECT
	public:
		QMutex mutex;
		QMutex inUse;
		QProcess *process;
		int sock, no;
		bool valid;
		aRtsDevice();
		~aRtsDevice();
		void deleteLater2();

	public slots:
		void processExited();
};

class aRtsPlayerRecorder : public QObject
{
	Q_OBJECT
	private:
		QMutex poolmutex;
		QMutex busymutex;
		QValueList<aRtsDevice *> pool;
		QValueList<aRtsDevice *> busy;
		int num;
		bool finalizing;
	public:
		friend class aRtsDevice;
		aRtsPlayerRecorder(QObject *parent=0, const char *name=0);
		~aRtsPlayerRecorder();
	public slots:
		void openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device);
		void closeDevice(SoundDevice device);
		void playSample(SoundDevice device, const int16_t* data, int length, bool& result);
		void recordSample(SoundDevice device, int16_t* data, int length, bool& result);
		void setFlushingEnabled(SoundDevice device, bool enabled);
};


extern aRtsPlayerRecorder *arts_player_recorder;

#endif
