#ifndef KADU_DSP_SOUND_H
#define KADU_DSP_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include "../sound/sound.h"

class OSSPlayThread : public QThread
{
	public:
		QMutex mutex;
		QSemaphore *semaphore; //s³u¿y do powiadamiania o d¼wiêku w kolejce
		bool end;
		QStringList list;
		
		OSSPlayThread();
		~OSSPlayThread();
		void run();
};

class OSSPlayerSlots : public QObject
{
	Q_OBJECT

	private:
		OSSPlayThread *thread;

	private slots:
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

