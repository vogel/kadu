#ifndef KADU_DSP_SOUND_H
#define KADU_DSP_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include "../sound/sound.h"

class SndParams
{
	public:
		SndParams(QString fm, QString dev, bool volCntrl=false, float vol=1);
		SndParams(const SndParams &p);
		SndParams();

		QString filename;
		QString device;
		bool volumeControl;
		float volume;
};

class OSSPlayThread : public QThread
{
	public:
		QMutex mutex;
		QSemaphore *semaphore; //s³u¿y do powiadamiania o d¼wiêku w kolejce
		bool end;
		QValueList<SndParams> list;
		
		OSSPlayThread();
		~OSSPlayThread();
		void run();
	private:
		static bool play(const char *path, const char *device, bool volCntrl=false, float volume=1);
		static int write_all(int fd, const char *data, int length, int chunksize);
};

class OSSPlayerSlots : public QObject
{
	Q_OBJECT

	private:
		OSSPlayThread *thread;

	public slots:
		void play(const QString &s, bool volCntrl, double vol, const QString &device=QString::null);		
		void playSound(const QString &s, bool volCntrl, double vol);
		void openDevice(int sample_rate, int channels, SoundDevice& device);
		void closeDevice(SoundDevice device);
		void playSample(SoundDevice device, const int16_t* data, int length, bool& result);
		void recordSample(SoundDevice device, int16_t* data, int length, bool& result);		

	public:
		OSSPlayerSlots(QObject *parent=0, const char *name=0);
		~OSSPlayerSlots();
		bool error;
};

extern OSSPlayerSlots *oss_player_slots;

#endif
